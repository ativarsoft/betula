/* Copyright (C) 2023 Mateus de Lima Oliveira */

#include <pollen/pollen.h>

static int init(tmpl_ctx_t data, tmpl_cb_t cb)
{
	cb->add_control_flow(data, TMPL_TRUE);
	return 0;
}

static void quit(void) {}

const tmpl_plugin_record_t templatizer_plugin_v1 = {
	&init,
	&quit
};

