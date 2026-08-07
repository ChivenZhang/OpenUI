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

static lxb_status_t callback(const lxb_char_t* data, size_t len, void* ctx)
{
    printf("%.*s", (int)len, (const char*)data);

    return LXB_STATUS_OK;
}

static lxb_status_t traverse_style_cb(lxb_dom_element_t* element, const lxb_css_rule_declaration_t* declr, void* ctx, lxb_css_selector_specificity_t spec, bool is_weak)
{
    auto& depth = *(int*)ctx;

    std::cout
        << std::string(depth * 4,  ' ')
        << "-style: ";

    auto status = lxb_css_rule_declaration_serialize(declr, callback, nullptr);
    if (status != LXB_STATUS_OK) return EXIT_FAILURE;

    printf("\n");

    return LXB_STATUS_OK;
}

void traverse_html_cb(lxb_dom_node_t* node, int depth = 0)
{
    while (node)
    {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT)
        {
            lxb_dom_element_t* element = lxb_dom_interface_element(node);

            size_t tag_len;
            const lxb_char_t* tag = lxb_dom_element_local_name(element, &tag_len);

            std::cout
                << std::string(depth * 4, ' ')
                << "<"
                << std::string(reinterpret_cast<const char*>(tag), tag_len)
                << ">\n";

            for (auto attr = element->first_attr; attr != nullptr; attr = attr->next)
            {
                size_t name_len;
                const lxb_char_t* name;

                name = lxb_dom_attr_local_name(attr, &name_len);

                std::cout
                    << std::string(depth * 4, ' ')
                    << "-attr : "
                    << std::string(reinterpret_cast<const char*>(name), name_len);
                if (attr->value)
                    std::cout
                    << " = \""
                    << std::string(reinterpret_cast<const char*>(attr->value->data), attr->value->length)
                    << "\"";

                std::cout << "\n";
            }

            lxb_dom_element_style_walk(element, traverse_style_cb, &depth, true);
        }

        if (node->first_child)
        {
            traverse_html_cb(node->first_child, depth + 1);
        }

        node = node->next;
    }
}

UIRef<UIParser::elem_t> UIParser::parse(UIString html) const
{
    auto document = lxb_html_document_create();
    if (document == nullptr) return {};

    /* Init all CSS objects and momory for Document. */

    auto status = lxb_style_init(document);
    if (status != LXB_STATUS_OK) return {};

    /* Parse HTML. */

    status = lxb_html_document_parse(document, (const lxb_char_t*)html.data(), html.length());
    if (status != LXB_STATUS_OK) return {};

    /* Create CSS parser. */

    auto parser = lxb_css_parser_create();
    status = lxb_css_parser_init(parser, nullptr);
    if (status != LXB_STATUS_OK) return {};

    /*
    auto styles = lxb_css_stylesheet_create(nullptr);
    status = lxb_css_stylesheet_parse(styles, parser, css.data, css.length);
    if (status != LXB_STATUS_OK) return {};
    status = lxb_html_document_stylesheet_attach(document, styles);
    if (status != LXB_STATUS_OK) return {};
    */

    traverse_html_cb(lxb_dom_interface_node(document));

    /* Destroy resources. */

    //(void) lxb_css_stylesheet_destroy(styles, true);
    (void)lxb_css_parser_destroy(parser, true);
    (void)lxb_style_destroy(document);
    (void)lxb_html_document_destroy(document);

    return {};
}
