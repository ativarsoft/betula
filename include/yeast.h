/* Copyright (C) 2023 Mateus de Lima Oliveira */

/*
 * Yeast
 *
 * A runtime for creating portable code.
 *
 */

#ifndef YEAST_H
#define YEAST_H

#include <stdio.h>

#define TMPL_UNCHECKED_MUL(lhs, rhs) ((lhs) * (rhs))

typedef const char *string_t;
typedef char *chars_ptr_t;
typedef FILE *file_t;
typedef void *void_ptr_t;

extern string_t config_codegen;

string_t yeast_get_home();

int read_config_file();

#define FREE_STRING(ctx, string)                \
    do {                                        \
        string_t s = string;                    \
        templatizer_free(ctx, (void_ptr_t) s);  \
    } while(0);

#endif

