#ifndef POLLEN_H
#define POLLEN_H

#include <templatizer.h>
#include <stdio.h>

typedef const char *string_t;
typedef FILE *file_t;
typedef void *void_ptr_t;

extern string_t config_codegen;

void_ptr_t load_library(tmpl_ctx_t data, string_t path);
int read_config_file();

#endif
