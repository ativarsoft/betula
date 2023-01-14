#include "templatizer.h"

static int init(struct context *data, struct templatizer_callbacks *cb)
{
	cb->add_control_flow(data, TMPL_FALSE);
	return 0;
}

static void quit() {}

struct templatizer_plugin templatizer_plugin_v1 = {
	&init,
	&quit
};
