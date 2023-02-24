#ifndef POLLEN_H
#define POLLEN_H

#include <templatizer.h>

extern char *config_codegen;

void *load_library(tmpl_ctx_t data, const char *path);
int read_config_file();

typedef const char *string_t;

#endif
