/* Copyright (C) 2023 Mateus de Lima Oliveira */

#include <pollen/pollen.h>

#define VERSION "1.0"

static int init(ctx, cb)
tmpl_ctx_t ctx;
tmpl_cb_t cb;
{
	const char *copyright = cb->get_copyright_string();
	cb->add_filler_text(ctx, copyright);
	return 0;
}

static void quit() {}

const tmpl_plugin_record_t templatizer_plugin_v1 = {
	&init,
	&quit
};
