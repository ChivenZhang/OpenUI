/*=================================================
* Copyright © 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#include "../UIParser.h"
#include <lexbor/core/array.h>
#include <lexbor/css/css.h>
#include <lexbor/html/html.h>
#include <lexbor/style/style.h>

namespace
{
using Widget = UIParser::widget_t;
using Style = UIParser::style_t;

struct DocumentDeleter
{
    void operator()(lxb_html_document_t* document) const
    {
        lxb_style_destroy(document);
        lxb_html_document_destroy(document);
    }
};

UIStringView stringView(const lxb_char_t* data, size_t length)
{
    return data ? UIStringView(reinterpret_cast<const char*>(data), length) : UIStringView{};
}

lxb_status_t appendString(const lxb_char_t* data, size_t length, void* context)
{
    static_cast<UIString*>(context)->append(stringView(data, length));
    return LXB_STATUS_OK;
}

lxb_status_t serializeDeclaration(const lxb_css_rule_declaration_t* declaration, Style& style)
{
    auto status = lxb_css_property_serialize_name(declaration->u.user, declaration->type, appendString, &style.Name);
    if (status != LXB_STATUS_OK) return status;
    return lxb_css_property_serialize(declaration->u.user, declaration->type, appendString, &style.Value);
}

lxb_status_t collectStyle(lxb_dom_element_t*, const lxb_css_rule_declaration_t* declaration,
    void* context, lxb_css_selector_specificity_t specificity, bool isWeak)
{
    if (isWeak) return LXB_STATUS_OK;

    Style style{};
    auto status = serializeDeclaration(declaration, style);
    if (status != LXB_STATUS_OK) return status;
    style.Priority = specificity;
    static_cast<Widget*>(context)->Style.push_back(std::move(style));
    return LXB_STATUS_OK;
}

bool isDynamicState(const lxb_css_selector_t* selector)
{
    switch (selector->u.pseudo.type)
    {
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_ACTIVE:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_CHECKED:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_CURRENT:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_DEFAULT:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_DISABLED:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_ENABLED:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_FOCUS:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_FOCUS_VISIBLE:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_FOCUS_WITHIN:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_FULLSCREEN:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUTURE:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_HOVER:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_IN_RANGE:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_INDETERMINATE:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_INVALID:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_OPTIONAL:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_OUT_OF_RANGE:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_PAST:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_PLACEHOLDER_SHOWN:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_READ_ONLY:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_READ_WRITE:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_REQUIRED:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_TARGET:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_TARGET_WITHIN:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_USER_INVALID:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_VALID:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_VISITED:
    case LXB_CSS_SELECTOR_PSEUDO_CLASS_WARNING:
        return true;
    default:
        return false;
    }
}

struct StateRule
{
    lxb_css_selector_list_t* Selectors;
    UIList<lxb_css_selector_t*> DynamicSelectors;
    UIHashMap<const lxb_css_selector_list_t*, UIString> States;
    UIList<Style> Declarations;
};

// Only mask runtime states; structural selectors must still match the DOM.
class StateMask
{
public:
    explicit StateMask(const UIList<lxb_css_selector_t*>& selectors) : m_Selectors(selectors)
    {
        for (auto selector : m_Selectors) selector->type = LXB_CSS_SELECTOR_TYPE_ANY;
    }

    ~StateMask()
    {
        for (auto selector : m_Selectors) selector->type = LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS;
    }

    StateMask(const StateMask&) = delete;
    StateMask& operator=(const StateMask&) = delete;

private:
    const UIList<lxb_css_selector_t*>& m_Selectors;
};

void collectStateRules(lxb_css_rule_t* rule, UIList<StateRule>& rules)
{
    for (; rule; rule = rule->next)
    {
        if (rule->type == LXB_CSS_RULE_AT_RULE)
        {
            auto at = lxb_css_rule_at(rule);
            if (at->type == LXB_CSS_AT_RULE_MEDIA && at->u.media && at->u.media->block)
                collectStateRules(at->u.media->block->first, rules);
            continue;
        }
        if (rule->type != LXB_CSS_RULE_STYLE) continue;

        auto styleRule = lxb_css_rule_style(rule);
        if (styleRule->selector == nullptr || styleRule->declarations == nullptr) continue;

        StateRule stateRule{};
        stateRule.Selectors = styleRule->selector;
        for (auto list = styleRule->selector; list; list = list->next)
        {
            UIString state;
            for (auto selector = list->first; selector; selector = selector->next)
            {
                if (selector->type != LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS) continue;
                if (isDynamicState(selector)) stateRule.DynamicSelectors.push_back(selector);
                if (selector->name.data == nullptr) continue;
                if (!state.empty()) state.push_back(':');
                state.append(stringView(selector->name.data, selector->name.length));
            }
            if (!state.empty()) stateRule.States.emplace(list, std::move(state));
        }
        if (stateRule.States.empty()) continue;

        // Serialize once per rule, instead of once per matching element.
        for (auto declarationRule = styleRule->declarations->first; declarationRule; declarationRule = declarationRule->next)
        {
            if (declarationRule->type != LXB_CSS_RULE_DECLARATION) continue;
            auto declaration = lxb_css_rule_declaration(declarationRule);
            Style style{};
            if (serializeDeclaration(declaration, style) != LXB_STATUS_OK || style.Name.empty()) continue;
            if (declaration->important) style.Priority = lxb_css_selector_sp_up_i(0);
            stateRule.Declarations.push_back(std::move(style));
        }
        if (!stateRule.Declarations.empty()) rules.push_back(std::move(stateRule));
    }
}

UIList<StateRule> collectStateRules(lxb_dom_document_css_t* css)
{
    UIList<StateRule> rules;
    if (css == nullptr || css->stylesheets == nullptr || css->selectors == nullptr) return rules;

    for (size_t index = 0; index < lexbor_array_length(css->stylesheets); ++index)
    {
        auto sheet = static_cast<lxb_css_stylesheet_t*>(lexbor_array_get(css->stylesheets, index));
        if (sheet && sheet->root && sheet->root->type == LXB_CSS_RULE_LIST)
            collectStateRules(lxb_css_rule_list(sheet->root)->first, rules);
    }
    return rules;
}

struct StateContext
{
    Widget& Target;
    const StateRule& Rule;
    lxb_selectors_t* Selectors;
};

lxb_status_t collectStateStyle(lxb_dom_node_t*, lxb_css_selector_specificity_t specificity, void* raw)
{
    auto& context = *static_cast<StateContext*>(raw);
    auto state = context.Rule.States.find(lxb_selectors_selector(context.Selectors));
    if (state == context.Rule.States.end()) return LXB_STATUS_OK;

    auto& styles = context.Target.Style;
    for (const auto& declaration : context.Rule.Declarations)
    {
        // For example: button:hover { font-size: 16px; } -> font-size:hover = 16px.
        auto name = declaration.Name + ':' + state->second;
        auto priority = specificity | declaration.Priority;
        auto found = std::find_if(styles.begin(), styles.end(), [&](const Style& style) { return style.Name == name; });
        if (found == styles.end()) styles.push_back({ std::move(name), declaration.Value, priority });
        else if (priority >= found->Priority)
        {
            found->Value = declaration.Value;
            found->Priority = priority;
        }
    }
    return LXB_STATUS_OK;
}

bool collectStyles(lxb_dom_element_t* element, Widget& widget, const UIList<StateRule>& rules, lxb_selectors_t* selectors)
{
    if (element->style && lxb_dom_element_style_walk(element, collectStyle, &widget, false) != LXB_STATUS_OK) return false;

    for (const auto& rule : rules)
    {
        StateMask mask(rule.DynamicSelectors);
        StateContext context{ widget, rule, selectors };
        if (lxb_selectors_match_node(selectors, lxb_dom_interface_node(element), rule.Selectors,
            collectStateStyle, &context) != LXB_STATUS_OK) return false;
    }
    return true;
}

void collectAttributes(lxb_dom_element_t* element, Widget& widget)
{
    for (auto attribute = element->first_attr; attribute; attribute = attribute->next)
    {
        auto& result = widget.Attrib.emplace_back();
        size_t length = 0;
        auto name = lxb_dom_attr_local_name(attribute, &length);
        result.Name = stringView(name, length);
        auto value = lxb_dom_attr_value(attribute, &length);
        result.Value = stringView(value, length);

        if (result.Name == "id") widget.ID = result.Value;
        else if (result.Name == "class") widget.Class = result.Value;
    }
}

bool collectWidget(lxb_dom_node_t* node, Widget& widget, const UIList<StateRule>& rules, lxb_selectors_t* selectors)
{
    auto element = lxb_dom_interface_element(node);
    size_t length = 0;
    auto tag = lxb_dom_element_local_name(element, &length);
    widget.Type = stringView(tag, length);
    collectAttributes(element, widget);
    if (!collectStyles(element, widget, rules, selectors)) return false;

    for (auto child = node->first_child; child; child = child->next)
    {
        if (child->type == LXB_DOM_NODE_TYPE_ELEMENT)
        {
            if (!collectWidget(child, widget.Children.emplace_back(), rules, selectors)) return false;
        }
        else if (child->type == LXB_DOM_NODE_TYPE_TEXT)
        {
            const auto& text = lxb_dom_interface_text(child)->char_data.data;
            widget.Text = stringView(text.data, text.length);
        }
    }
    return true;
}
}

bool UIParser::parse(UIString html, widget_t& result) const
{
    std::unique_ptr<lxb_html_document_t, DocumentDeleter> document(lxb_html_document_create());
    if (!document || lxb_style_init(document.get()) != LXB_STATUS_OK) return false;
    if (lxb_html_document_parse(document.get(), reinterpret_cast<const lxb_char_t*>(html.data()), html.length()) != LXB_STATUS_OK)
        return false;
    if (document->body == nullptr) return false;

    auto css = document->dom_document.css;
    auto rules = collectStateRules(css);
    widget_t body;
    if (!collectWidget(lxb_dom_interface_node(document->body), body, rules, css ? css->selectors : nullptr)) return false;

    // Publish only a complete tree; keep the existing append-to-result behavior.
    result.Children.push_back(std::move(body));
    return true;
}
