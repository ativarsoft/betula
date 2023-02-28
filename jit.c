/* Copyright (C) 2023 Mateus de Lima Oliveira */

#include "pollen.h"
#include "jit.h"
#include "config.h"

#ifdef DEBUG
string_t config_codegen = CONFIG_CODEGEN_PATH;
#else
string_t config_codegen = NULL;
#endif

tmpl_codegen_t pollen_codegen_new(tmpl_ctx_t ctx)
{
    tmpl_codegen_t codegen = NEW_CODEGEN_PLUGIN(ctx);
    if (codegen == NULL) {
        return NULL;
    }
    CODEGEN_SET_CONTEXT(codegen, ctx);
    return codegen;
}

static void error_no_codegen()
{
    fputs("Error: codegen is null.\n", stderr);
    abort();
}

int pollen_codegen_sanity_check(tmpl_ctx_t ctx)
{
    tmpl_codegen_t codegen = tmpl_get_codegen(ctx);
    tmpl_lib_t handle = (codegen)? (CODEGEN_GET_LIBRARY(codegen)) : (error_no_codegen(), NULL);
    int rc = -1;

    if (handle == NULL) {
       fprintf(stderr,
               "Error: Unable to perform codegen sanity check because "
               "there is no codegen plugin loaded.");
       return -2;
    }

    tmpl_sym_t symbol = tmpl_get_symbol(handle, "pollen_codegen_sanity_check");
    if (symbol == NULL)
        return -3; // Failed to get symbol

    /* Call the symbol as a function */
    POLLEN_CALL_SYMBOL_0(symbol, rc);
    return rc;
}

int pollen_codegen_init(tmpl_ctx_t ctx)
{
	string_t path = config_codegen;
	tmpl_codegen_t codegen = NULL;
	tmpl_lib_t library = {0};

	if (ctx == NULL)
		return 1;

	if (path == NULL)
		return 0; /* No plugin selected, therefore no error. */

	codegen = pollen_codegen_new(ctx);
	if (codegen == NULL) {
		fprintf(stderr, "Error: failed to allocate memory for codegen record.\n");
		return 3;
	}

	library = tmpl_load_library(ctx, path);
	if (library == NULL) {
		fprintf(stderr, "Failed to load codegen plugin \"%s\".\n", path);
		return 4;
	}

	CODEGEN_SET_LIBRARY(codegen, library);
	tmpl_set_codegen(ctx, codegen);

	return 0;
}

int pollen_codegen_quit(tmpl_codegen_t codegen)
{
    tmpl_lib_t library = NULL;

    if (codegen == NULL)
    	return 0;

    library = CODEGEN_GET_LIBRARY(codegen);

    /* Unload shared object file */
    if (tmpl_unload_library(library) != 0) {
        fputs("Failed to unload codegen plugin.\n", stderr);
        tmpl_set_codegen(CODEGEN_GET_CONTEXT(codegen), NULL);
        return 3;
    }
    tmpl_set_codegen(CODEGEN_GET_CONTEXT(codegen), NULL);
    return 0;
}

