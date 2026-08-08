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
#include <lexbor/selectors/selectors.h>

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

    auto foundBody = false;

    UILambda<void(lxb_dom_node_t*, widget_t*)> dom_func;
    dom_func = [&dom_func, &foundBody](lxb_dom_node_t* node, widget_t* parent)
    {
        if (node->type == LXB_DOM_NODE_TYPE_DOCUMENT)
        {
            auto document = lxb_dom_interface_document(node);

            for (auto child = node->first_child; child; child = child->next)
            {
                dom_func(child, parent);
            }
        }
        else if (node->type == LXB_DOM_NODE_TYPE_ELEMENT)
        {
            auto element = lxb_dom_interface_element(node);

            size_t tag_len = 0;
            auto tag = lxb_dom_element_local_name(element, &tag_len);
            auto tagName = UIStringView(reinterpret_cast<const char*>(tag), tag_len);
            if (foundBody || tagName == "body") foundBody = true;

            if (foundBody)
            {
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

                for (auto child = node->first_child; child; child = child->next)
                {
                    dom_func(child, widget);
                }
            }
            else
            {
                for (auto child = node->first_child; child; child = child->next)
                {
                    dom_func(child, parent);
                }
            }
        }
        else if (node->type == LXB_DOM_NODE_TYPE_TEXT)
        {
            auto text = lxb_dom_interface_text(node);
            auto textValue = UIString(reinterpret_cast<const char*>(text->char_data.data.data), text->char_data.data.length);

            parent->Text = textValue;
        }
    };
    dom_func(lxb_dom_interface_node(document), &result);

    /* Destroy resources. */

    //(void) lxb_css_stylesheet_destroy(styles, true);
    (void)lxb_css_parser_destroy(parser, true);
    (void)lxb_style_destroy(document);
    (void)lxb_html_document_destroy(document);

    return true;
}