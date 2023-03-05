/* Copyright (C) 2023 Mateus de Lima Oliveira */

#include <pollen/pollen.h>
#include <stdio.h>

#define QLIST_FOREACH(a, b, c)

struct parameter {
	char *type;
	char *name;
};

static int gen_function_start(ctx, el, attr)
tmpl_ctx_t ctx;
const char *el;
const char **attr;
{
	char *func_name = "";
	char *func_ret_type = "";
	struct parameter *arg;
	fputs(func_ret_type, stdout);
	fputc(' ', stdout);
	fputs(func_name, stdout);
	fputc('(', stdout);
	QLIST_FOREACH (arg, arg_list, entry) {
		fputs(arg->type, stdout);
		fputc(' ', stdout);
		fputs(arg->name, stdout);
	}
	fputc(')', stdout);
	return 0;
}

static int gen_function_end()
{
	fputs("}", stdout);
	return 0;
}

static int gen_if_start(ctx, el, attr)
tmpl_ctx_t ctx;
const char *el;
const char **attr;
{
	fputs("if", stdout);
	fputc(' ', stdout);
	fputc('{', stdout);
	fputc('\n', stdout);
	return 0;
}

static int gen_if_end(ctx, el)
tmpl_ctx_t ctx;
const char *el;
{
	fputc('}', stdout);
	fputc('\n', stdout);
	return 0;
}

static int gen_swhile_start(ctx, el, attr)
tmpl_ctx_t ctx;
const char *el;
const char **attr;
{
	fputs("while", stdout);
	fputc(' ', stdout);
	fputc('{', stdout);
	fputc('\n', stdout);
	return 0;
}

static int gen_swhile_end(ctx, el)
tmpl_ctx_t ctx;
const char *el;
{
	return 0;
}

static int gen_ewhile_start(ctx, el, attr)
tmpl_ctx_t ctx;
const char *el;
const char **attr;
{
	fputc('}', stdout);
	fputc('\n', stdout);
	return 0;
}

static int gen_ewhile_end(ctx, el)
tmpl_ctx_t ctx;
const char *el;
{
	return 0;
}

static int init(tmpl_ctx_t data, tmpl_cb_t cb)
{
	cb->register_element_start_tag
		(data, "if",
		&gen_swhile_start);
	cb->register_element_end_tag
		(data, "if",
		&gen_swhile_end);
	cb->register_element_start_tag
		(data, "swhile",
		&gen_swhile_start);
	cb->register_element_end_tag
		(data, "swhile",
		&gen_swhile_end);
	cb->register_element_start_tag
		(data, "ewhile",
		&gen_ewhile_start);
	cb->register_element_end_tag
		(data, "ewhile",
		&gen_ewhile_end);
	return 0;
}

static void quit() {}

const tmpl_plugin_record_t templatizer_plugin_v1 = {
	&init,
	&quit
};
