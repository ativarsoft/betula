/* Copyright (C) 2023 Mateus de Lima Oliveira */

// This header guard prevents multiple inclusion of this file
#ifndef POLLEN_JIT_H
#define POLLEN_JIT_H

#include <string.h> /* memset() */
#include <assert.h>
#include "pollen.h"

extern string_t config_codegen;

/* Macro to call a function with no arguments */
#define POLLEN_CALL_SYMBOL_0(symbol, rc)                   \
    do {                                                   \
        int (*fn_ptr)() = (int (*)()) symbol;              \
        rc = fn_ptr();                                     \
    } while(0);

#define NEW_CODEGEN_PLUGIN(ctx)                          \
    (tmpl_codegen_t) templatizer_malloc(ctx, sizeof(struct pollen_codegen_plugin));

#define CODEGEN_SET_CONTEXT(codegen, x)                           \
    ((codegen != NULL)? (codegen->ctx = x) : (abort(), NULL))

#define CODEGEN_GET_CONTEXT(codegen)                              \
    ((codegen != NULL)? (codegen->ctx) : (abort(), NULL))

#define CODEGEN_SET_LIBRARY(codegen, lib)                         \
    ((codegen != NULL)? (codegen->library = lib) : (abort(), NULL))

/* Macro to get the library handle from the plugin context */
#define CODEGEN_GET_LIBRARY(codegen)                              \
    ((codegen != NULL)? (codegen->library) : (abort(), NULL))

/* Function prototypes */
int pollen_codegen_init(tmpl_ctx_t ctx);
int pollen_codegen_quit(tmpl_codegen_t ctx);
int pollen_codegen_sanity_check(tmpl_ctx_t plugin);

/* End of header guard */
#endif

