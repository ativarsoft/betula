/* Copyright (C) 2023 Mateus de Lima Oliveira */

#include <pollen/pollen.h>

static int init()
{
	return 0;
}

static void quit() {}

const tmpl_plugin_record_t templatizer_plugin_v1 = {
	&init,
	&quit
};

