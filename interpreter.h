/* Copyright (C) 2017-2022 Mateus de Lima Oliveira */

#ifndef TMPL_INTERPRETER_H
#define TMPL_INTERPRETER_H

#include <templatizer.h>
#include <stdbool.h>

void print_list(tmpl_ctx_t data);
bool is_control_flow_keyword(const XML_Char *el);

#endif
