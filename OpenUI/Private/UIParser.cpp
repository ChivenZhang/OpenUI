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
#include <lexbor/css/css.h>
#include <lexbor/html/html.h>
#include <lexbor/style/style.h>
#include <lexbor/core/array.h>

bool UIParser::parse(UIString html, widget_t& result) const
{
    /* Create Document. */

    auto document = lxb_html_document_create();
    if (document == nullptr) return false;

    /* Init Style/CSS. */

    auto status = lxb_style_init(document);
    if (status != LXB_STATUS_OK) return false;

    /* Parse HTML. */

    status = lxb_html_document_parse(document, (const lxb_char_t*)html.data(), html.length());
    if (status != LXB_STATUS_OK) return false;

    /* Create CSS parser. */

    auto parser = lxb_css_parser_create();
    status = lxb_css_parser_init(parser, nullptr);
    if (status != LXB_STATUS_OK) return false;

    /*
    auto styles = lxb_css_stylesheet_create(nullptr);
    status = lxb_css_stylesheet_parse(styles, parser, css.data, css.length);
    if (status != LXB_STATUS_OK) return false;
    status = lxb_html_document_stylesheet_attach(document, styles);
    if (status != LXB_STATUS_OK) return false;
    */

    struct StateMask
    {
        struct item_t
        {
            lxb_css_selector_t* selector;
            lxb_css_selector_type_t type;
        };
        UIList<item_t> items;
        ~StateMask()
        {
            for (auto& item : items) item.selector->type = item.type;
        }
    };
    struct StateCtx
    {
        widget_t* widget;
        lxb_css_rule_style_t* style;
        StateMask* mask;
        lxb_selectors_t* selectors;
    };

    UILambda<void(lxb_dom_node_t*, widget_t*)> dom_func;
    dom_func = [&dom_func, document](lxb_dom_node_t* node, widget_t* parent)
    {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT)
        {
            auto element = lxb_dom_interface_element(node);

            size_t tag_len = 0;
            auto tag = lxb_dom_element_local_name(element, &tag_len);

            auto widget = &parent->Children.emplace_back();
            widget->Type = UIStringView(reinterpret_cast<const char*>(tag), tag_len);

            for (auto attr = element->first_attr; attr; attr = attr->next)
            {
                auto& attrib = widget->Attrib.emplace_back();

                size_t name_len;
                auto name = lxb_dom_attr_local_name(attr, &name_len);
                attrib.Name = UIStringView(reinterpret_cast<const char*>(name), name_len);

                size_t value_len;
                auto value = lxb_dom_attr_value(attr, &value_len);
                attrib.Value = UIStringView(reinterpret_cast<const char*>(value), value_len);

                if (attrib.Name == "id") widget->ID = attrib.Value;
                if (attrib.Name == "class") widget->Class = attrib.Value;
            }

            auto css_func = [](lxb_dom_element_t* element, const lxb_css_rule_declaration_t* decl, void* ctx, lxb_css_selector_specificity_t spec, bool is_weak)->lxb_status_t
            {
                auto widget = (widget_t*)ctx;
                if (is_weak) return LXB_STATUS_OK;

                UIString styleName;
                auto status = lxb_css_property_serialize_name(decl->u.user, decl->type,
                    [](const lxb_char_t *data, size_t len, void *ctx) -> lxb_status_t
                     {
                         auto& buffer = *(UIString*)ctx;
                         buffer += UIStringView(reinterpret_cast<const char*>(data), len);
                         return LXB_STATUS_OK;
                     }, &styleName);
                if (status != LXB_STATUS_OK) return EXIT_FAILURE;

                UIString styleValue;
                status = lxb_css_property_serialize(decl->u.user, decl->type,
                    [](const lxb_char_t *data, size_t len, void *ctx) -> lxb_status_t
                     {
                         auto& buffer = *(UIString*)ctx;
                         buffer += UIStringView(reinterpret_cast<const char*>(data), len);
                         return LXB_STATUS_OK;
                     }, &styleValue);
                if (status != LXB_STATUS_OK) return EXIT_FAILURE;

                auto& style = widget->Style.emplace_back();
                style.Name = styleName;
                style.Value = styleValue;
                style.Priority = spec;

                return LXB_STATUS_OK;
            };
            lxb_dom_element_style_walk(element, css_func, widget, true);

            auto pseudo_func = [](lxb_dom_node_t* node, lxb_css_selector_specificity_t spec, void* raw) -> lxb_status_t
            {
                auto context = (StateCtx*)raw;
                auto list = lxb_selectors_selector(context->selectors);
                if (list == nullptr || context->style->declarations == nullptr) return LXB_STATUS_OK;

                UIString state;
                for (auto* selector = list->first; selector; selector = selector->next)
                {
                    bool recorded = selector->type == LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS;
                    if (recorded == false)
                    {
                        for (auto& item : context->mask->items)
                        {
                            if (item.selector == selector) { recorded = true; break; }
                        }
                    }
                    if (recorded == false || selector->name.data == nullptr) continue;
                    if (state.empty() == false) state.push_back(':');
                    state.append(reinterpret_cast<const char*>(selector->name.data), selector->name.length);
                }
                if (state.empty()) return LXB_STATUS_OK;

                for (auto* declRule = context->style->declarations->first; declRule; declRule = declRule->next)
                {
                    if (declRule->type != LXB_CSS_RULE_DECLARATION) continue;
                    auto decl = lxb_css_rule_declaration(declRule);
                    UIString name, value;
                    if (lxb_css_property_serialize_name(decl->u.user, decl->type, [](const lxb_char_t *data, size_t len, void *ctx) -> lxb_status_t
                    {
                        auto& buffer = *(UIString*)ctx;
                        buffer += UIStringView(reinterpret_cast<const char*>(data), len);
                        return LXB_STATUS_OK;
                    }, &name) != LXB_STATUS_OK) continue;
                    if (lxb_css_property_serialize(decl->u.user, decl->type, [](const lxb_char_t *data, size_t len, void *ctx) -> lxb_status_t
                    {
                        auto& buffer = *(UIString*)ctx;
                        buffer += UIStringView(reinterpret_cast<const char*>(data), len);
                        return LXB_STATUS_OK;
                    }, &value) != LXB_STATUS_OK) continue;
                    if (name.empty()) continue;
                    name.push_back(':');
                    name += state;
                    auto& styles = context->widget->Style;
                    auto found = styles.end();
                    for (auto it = styles.begin(); it != styles.end(); ++it)
                    {
                        if (it->Name != name) continue;
                        found = it;
                        break;
                    }
                    if (found == styles.end()) styles.push_back({ std::move(name), std::move(value), spec });
                    else if (spec >= found->Priority)
                    {
                        found->Value = std::move(value);
                        found->Priority = spec;
                    }
                }
                return LXB_STATUS_OK;
            };

            auto css = document->dom_document.css;
            if (css != nullptr && css->stylesheets != nullptr && css->selectors != nullptr)
            {
                UILambda<void(lxb_css_rule_t*)> lxb_css_rule_selector_walk;
                lxb_css_rule_selector_walk = [&](lxb_css_rule_t* rule)
                {
                    for (; rule; rule = rule->next)
                    {
                        if (rule->type == LXB_CSS_RULE_STYLE)
                        {
                            auto styleRule = lxb_css_rule_style(rule);
                            if (styleRule == nullptr || styleRule->selector == nullptr) continue;

                            StateMask mask;
                            bool pseudo = false;
                            for (auto* current = styleRule->selector; current; current = current->next)
                            {
                                for (auto* selector = current->first; selector; selector = selector->next)
                                {
                                    if (selector->type != LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS) continue;
                                    pseudo = true;
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
                                        mask.items.push_back({ selector, selector->type });
                                        selector->type = LXB_CSS_SELECTOR_TYPE_ANY;
                                        break;
                                    default:
                                        break;
                                    }
                                }
                            }
                            if (pseudo == false) continue;

                            StateCtx ctx{ widget, styleRule, &mask, css->selectors };
                            lxb_selectors_match_node(css->selectors, lxb_dom_interface_node(element), styleRule->selector, pseudo_func, &ctx);
                        }
                        else if (rule->type == LXB_CSS_RULE_AT_RULE)
                        {
                            auto at = lxb_css_rule_at(rule);
                            if (at->type == LXB_CSS_AT_RULE_MEDIA && at->u.media && at->u.media->block)
                                lxb_css_rule_selector_walk(at->u.media->block->first);
                        }
                    }
                };

                for (size_t index = 0; index < lexbor_array_length(css->stylesheets); ++index)
                {
                    auto sheet = (lxb_css_stylesheet_t*)lexbor_array_get(css->stylesheets, index);
                    if (sheet == nullptr || sheet->root == nullptr || sheet->root->type != LXB_CSS_RULE_LIST) continue;
                    lxb_css_rule_selector_walk(lxb_css_rule_list(sheet->root)->first);
                }
            }

            for (auto child = node->first_child; child; child = child->next)
            {
                dom_func(child, widget);
            }
        }
        else if (node->type == LXB_DOM_NODE_TYPE_TEXT)
        {
            auto text = lxb_dom_interface_text(node);
            auto textValue = UIString(reinterpret_cast<const char*>(text->char_data.data.data), text->char_data.data.length);

            parent->Text = textValue;
        }
    };
    dom_func(lxb_dom_interface_node(document->body), &result);

    /* Destroy resources. */

    //(void) lxb_css_stylesheet_destroy(styles, true);
    (void)lxb_css_parser_destroy(parser, true);
    (void)lxb_style_destroy(document);
    (void)lxb_html_document_destroy(document);

    return true;
}
