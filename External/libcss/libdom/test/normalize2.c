/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2024 Daniel Silverstone <dsilvers@netsurf-browser.org>
 */

#include <stdio.h>
#include <stdbool.h>

#include "testutils/domts.h"

static dom_exception
get_node(dom_document *doc, dom_string *tag, dom_node **out)
{
	dom_nodelist *nodes;
	dom_exception err;
	dom_node *docele;
	dom_node *ret;

	err = dom_document_get_document_element(doc, &docele);
	if (err != DOM_NO_ERR) {
		printf("Could not get document node\n");
		return err;
	}
	err = dom_element_get_elements_by_tag_name(docele, tag, &nodes);
	dom_node_unref(docele);
	if (err != DOM_NO_ERR) {
		printf("Could not enumerate elements for %*s\n",
		       dom_string_length(tag),
		       dom_string_data(tag));
		return err;
	}

	err = dom_nodelist_item(nodes, 0, &ret);
	dom_nodelist_unref(nodes);
	if (err != DOM_NO_ERR) {
		printf("Could not retrieve element[0] for %*s\n",
		       dom_string_length(tag),
		       dom_string_data(tag));
		return err;
	}

	*out = ret;

	return DOM_NO_ERR;
}

static bool test_normalize(const char *fname)
{
	bool outcome = true;
	dom_document *doc = NULL;
	dom_string *domBODY = NULL;
	dom_node *body = NULL, *p = NULL, *text = NULL;
	dom_node_type type;
	dom_exception err = DOM_NO_ERR;

	printf("Loading: %s\n", fname);
	doc = load_html(fname, false);
	if (doc == NULL) {
		printf("Failed to load file\n");
		outcome = false;
		goto cleanup;
	}

	/* We have an HTML document so we normalise the body */

	err = dom_string_create((uint8_t *)"BODY", 4, &domBODY);
	if (err != DOM_NO_ERR) {
		printf("Failed to create BODY string\n");
		outcome = false;
		goto cleanup;
	}

	err = get_node(doc, domBODY, &body);
	if (err != DOM_NO_ERR) {
		outcome = false;
		goto cleanup;
	}

	err = dom_node_get_first_child(body, &p);
	if (err != DOM_NO_ERR) {
		outcome = false;
		goto cleanup;
	}

	err = dom_node_get_node_type(p, &type);
	if (err != DOM_NO_ERR) {
		outcome = false;
		goto cleanup;
	}

	while (p != NULL && type != DOM_ELEMENT_NODE) {
		dom_node *next = NULL;
		err = dom_node_get_next_sibling(p, &next);
		if (err != DOM_NO_ERR) {
			outcome = false;
			goto cleanup;
		}
		dom_node_unref(p);
		p = next;
		err = dom_node_get_node_type(p, &type);
		if (err != DOM_NO_ERR) {
			outcome = false;
			goto cleanup;
		}
	}

	err = dom_node_get_last_child(p, &text);
	if (err != DOM_NO_ERR) {
		outcome = false;
		goto cleanup;
	}

	printf("Normalizing body, please wait...\n");
	err = dom_node_normalize(body);
	if (err != DOM_NO_ERR) {
		printf("Failed to normalize body\n");
		outcome = false;
		goto cleanup;
	}

	err = dom_node_get_node_type(text, &type);
	if (err != DOM_NO_ERR) {
		outcome = false;
		goto cleanup;
	}

	printf("All done\n");

cleanup:
	if (err != DOM_NO_ERR) {
		printf("DOM Exception: %d\n", err);
	}
	dom_node_unref(text);
	dom_node_unref(p);
	dom_node_unref(body);
	dom_string_unref(domBODY);
	dom_node_unref(doc);
	return outcome;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "usage: %s inputfile", argv[0]);
		return 1;
	}

	/* This test simply loads the HTML and normalises the top node */
	if (!test_normalize(argv[1])) {
		printf("\nFAILED\n");
		return 1;
	}

	printf("\nPASS\n");
	return 0;
}
