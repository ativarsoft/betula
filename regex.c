/* Copyright (C) 2023 Mateus de Lima Oliveira */

/*
 * templatizer/regex.c:
 *
 * Regular expression support.
 * This is very useful for plugins that parse text.
 */

#include <templatizer.h>
#include <assert.h>
#include "memory.h"
#include "tmplregex.h"

int tmpl_regex_compile(tmpl_ctx_t ctx, tmpl_regex_t *obj, const char *pattern)
{
	tmpl_regex_t p = NULL;
	regex_t compiled;
	int rc = -1;
	p = (tmpl_regex_t) tmpl_process_malloc(ctx, sizeof(*p));
	rc = regcomp(&compiled, pattern, 0);
	p->compiled = compiled;
	*obj = p;
	return 0;
}

int tmpl_regex_execute(tmpl_regex_t obj, const char *string)
{
	tmpl_ctx_t tmpl = NULL;
	regex_t compiled;
	int rc = -1;
	assert(obj != NULL);
	tmpl = obj->tmpl;
	assert(tmpl != NULL);
	compiled = obj->compiled;
	rc = regexec(&compiled, string, 0, NULL, 0);
	obj->rc = rc;
	return 0;
}

int get_result(tmpl_regex_t obj)
{
	int rc = -1;
	assert(obj != NULL);
	switch (obj->rc) {
		default:
		rc = obj->rc;
		break;
	}
	return rc;
}

int tmpl_regex_cleanup()
{
	return 0;
}
