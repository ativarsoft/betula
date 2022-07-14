#include "templatizer.h"

static int init(struct context *data, struct templatizer_callbacks *cb)
{
	cb->send_default_headers(data);
	cb->add_filler_text(data, "Hello world!");
	return 0;
}

static void quit() {}

struct templatizer_plugin templatizer_plugin_v1 = {
	&init,
	&quit
};
