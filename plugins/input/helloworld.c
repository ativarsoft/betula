/* Copyright (C) 2023 Mateus de Lima Oliveira */

#include <pollen/pollen.h>

static int init(tmpl_ctx_t data, tmpl_cb_t cb)
{
	cb->send_default_headers(data);
	cb->add_filler_text(data, "Hello world!");
	return 0;
}

static void quit() {}

const tmpl_plugin_record_t templatizer_plugin_v1 = {
	&init,
	&quit
};
