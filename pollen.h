#ifndef PRIVATE_POLLEN_H
#define PRIVATE_POLLEN_H

#include <templatizer.h>
#include <yeast.h>

/* Check if we are running on Windows */
#ifdef _WIN32
#error Windows is not supported yet.
#else

/* Define a type for the library handle based on the platform */
typedef void *tmpl_lib_t;

/* This line defines a type called tmpl_sym_t as a pointer to void.
 * This is typically used to represent symbols from libraries. */
typedef void *tmpl_sym_t;

#endif

#define POLLEN_GET_CODEGEN(tmpl) \
    ((tmpl != NULL)? (tmpl->codegen) : (abort(), NULL))
#define POLLEN_SET_CODEGEN(tmpl, x) \
    ((tmpl != NULL)? (tmpl->codegen = x) : (abort(), NULL))
#define POLLEN_GET_PLUGIN_HANDLE(tmpl) \
    ((tmpl != NULL)? (tmpl->plugin_handle) : (abort(), NULL))

/* Define a type for the codegen plugin context */
typedef struct pollen_codegen_plugin {
    tmpl_ctx_t ctx;
    /* This is the parent context structure. */

    tmpl_lib_t library;
} *tmpl_codegen_t;

void_ptr_t templatizer_malloc(tmpl_ctx_t data, size_t size);

tmpl_lib_t tmpl_load_library(tmpl_ctx_t data, string_t path);
int tmpl_unload_library(tmpl_lib_t handle);
tmpl_sym_t tmpl_get_symbol(tmpl_lib_t handle, string_t name);

tmpl_codegen_t tmpl_get_codegen(tmpl_ctx_t ctx);
tmpl_codegen_t tmpl_set_codegen(tmpl_ctx_t ctx, tmpl_codegen_t codegen);

/* For the configuration parser (lex and yacc) */
int set_config_option_string(string_t name, string_t value);
int set_config_option_int(string_t name, int value);

#endif

