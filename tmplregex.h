/* Copyright (C) 2023 Mateus de Lima Oliveira */

#ifndef TMPL_REGEX_H
#define TMPL_REGEX_H

#include "config.h"

#ifdef HAS_REGEX

#include <templatizer.h>

#define _POSIX_C_SOURCE
#include <regex.h>

/*
 * TODO:
 * - Free regex automatically with Apache APR
 */

typedef struct {
        tmpl_ctx_t tmpl;
	/* This is NULL if this object has already
	 * been freed. */

        regex_t compiled;
	int rc;
} *tmpl_regex_t;

int tmpl_regex_compile(tmpl_ctx_t ctx, tmpl_regex_t *obj, const char *pattern);
int tmpl_regex_execute(tmpl_regex_t obj, const char *string);
int tmpl_regex_cleanup();

#endif

#endif
