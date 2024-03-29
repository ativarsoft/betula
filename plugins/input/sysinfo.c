/* Copyright (C) 2023 Mateus de Lima Oliveira */

#include <pollen/pollen.h>
#include <pollen/system.h>

#define BUFFER_SIZE 16

static int sysinfo(tmpl_ctx_t ctx, tmpl_cb_t cb)
{
	char buffer[BUFFER_SIZE];
	int num_processors = tmpl_get_num_processors();
	if (num_processors <= 0)
		return 2;
	snprintf(buffer, BUFFER_SIZE, "%d", num_processors);
	cb->add_filler_text(ctx, buffer);
	return 0;
}

static int init(tmpl_ctx_t ctx, tmpl_cb_t cb)
{
	return sysinfo(ctx, cb);
}

static void quit(void) {}

const tmpl_plugin_record_t templatizer_plugin_v1 = {
	.init = &init,
	.quit = &quit
};

