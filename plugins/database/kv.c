#include "templatizer.h"

TMPL_CALLBACKS(tmpl);

int create_kv_object(tmpl_ctx_t ctx)
{
	return 0;
}

int on_kv_start_tag(tmpl_ctx_t ctx, const char *el, const char **attr)
{
	int handle;
	handle = create_kv_object(ctx);
	tmpl->exit(ctx, handle);
	return 0;
}

static int init(tmpl_ctx_t ctx, struct templatizer_callbacks *cb)
{
	TMPL_INIT_CALLBACKS(tmpl, cb);
	tmpl->send_default_headers(ctx);
	tmpl->add_filler_text(ctx, "Hello world!");
	tmpl->register_element_start_tag(ctx, "kv", &on_kv_start_tag);
	return 0;
}

static void quit() {}

struct templatizer_plugin templatizer_plugin_v1 = {
	&init,
	&quit,
};
