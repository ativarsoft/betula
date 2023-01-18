/* Copyright (C) 2023 Mateus de Lima Oliveira */

#ifndef TMPL_MEMORY_H
#define TMPL_MEMORY_H

void *templatizer_malloc(tmpl_ctx_t data, size_t size);
void templatizer_free(tmpl_ctx_t data, void *ptr);

void *tmpl_process_malloc(tmpl_ctx_t ctx, size_t size);

#endif

