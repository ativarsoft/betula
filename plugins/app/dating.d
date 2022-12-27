/* Copyright (C) 2022 Mateus de Lima Oliveira */

import templatizer;

extern(C)
static int init(tmpl_ctx_t data, tmpl_cb_t cb)
{
	return 0;
}

extern(C)
static void quit() {}

extern(C)
templatizer_plugin templatizer_plugin_v1 = {
	&init,
	&quit
};
