/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2024 Daniel Silverstone <dsilvers@netsurf-browser.org>
 */

#include <stdio.h>
#include <stdbool.h>

#include "testutils/domts.h"

#if defined(__has_feature)
# if __has_feature(address_sanitizer)
#  if !defined(__SANITIZE_ADDRESS__)
#    define __SANITIZE_ADDRESS__
#  endif
# endif
#endif

#if defined(__linux__) && !defined(__SANITIZE_ADDRESS__)

#include <sys/time.h>
#include <sys/resource.h>

#define SOFT_LIMIT (128ul * 1024ul * 1024ul)
#define HARD_LIMIT_MUL (4ul)

static void limit_ram(void)
{
	struct rlimit lim;

	if (getrlimit(RLIMIT_DATA, &lim) == -1) {
		perror("Unable to get RLIMIT_DATA");
	} else {
		printf("Initial limits were: soft=%lu hard=%lu\n",
		       lim.rlim_cur,
		       lim.rlim_max);
	}

	lim.rlim_cur = SOFT_LIMIT;
	lim.rlim_max = lim.rlim_cur * HARD_LIMIT_MUL;

	if (setrlimit(RLIMIT_DATA, &lim) == -1) {
		perror("Unable to set RLIMIT_DATA");
	} else {
		printf("Set limits to: soft=%lu hard=%lu\n",
		       lim.rlim_cur,
		       lim.rlim_max);
	}
}

#else

static void limit_ram(void)
{
	printf("Cannot limit RAM, risky execution proceeds...\n");
}
#endif

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
	dom_string *domHEAD = NULL, *domBODY = NULL;
	dom_node *html = NULL, *head = NULL, *body = NULL;
	dom_exception err = DOM_NO_ERR;

	limit_ram();

	printf("Loading: %s\n", fname);
	doc = load_html(fname, false);
	if (doc == NULL) {
		printf("Failed to load file\n");
		outcome = false;
		goto cleanup;
	}

	/* We have an HTML document so we normalise the head and the body */

	err = dom_string_create((uint8_t *)"HEAD", 4, &domHEAD);
	if (err != DOM_NO_ERR) {
		printf("Failed to create HEAD string\n");
		outcome = false;
		goto cleanup;
	}

	err = dom_string_create((uint8_t *)"BODY", 4, &domBODY);
	if (err != DOM_NO_ERR) {
		printf("Failed to create BODY string\n");
		outcome = false;
		goto cleanup;
	}

	err = get_node(doc, domHEAD, &head);
	if (err != DOM_NO_ERR) {
		outcome = false;
		goto cleanup;
	}

	err = get_node(doc, domBODY, &body);
	if (err != DOM_NO_ERR) {
		outcome = false;
		goto cleanup;
	}

	printf("Normalizing head, please wait...\n");
	err = dom_node_normalize(head);
	if (err != DOM_NO_ERR) {
		printf("Failed to normalize head\n");
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

	printf("All done\n");

cleanup:
	if (err != DOM_NO_ERR) {
		printf("DOM Exception: %d\n", err);
	}
	if (head != NULL) {
		dom_node_unref(head);
	}
	if (html != NULL) {
		dom_node_unref(html);
	}
	if (domBODY != NULL) {
		dom_string_unref(domBODY);
	}
	if (domHEAD != NULL) {
		dom_string_unref(domHEAD);
	}
	if (doc != NULL) {
		dom_node_unref(doc);
		doc = NULL;
	}
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
