#include <templatizer.h>
#include <threads.h>

thread_local tmpl_cb_t kv_cb = NULL;

int kv_get()
{
	/* TODO: return actual data from database to a placeholder. */
	return 0;
}

static int create_kv_object(tmpl_ctx_t ctx)
{
	return 0;
}

static int on_kv_start_tag(tmpl_ctx_t ctx, const char *el, const char **attr)
{
	int handle;
	handle = create_kv_object(ctx);
	return 0;
}

static int init(tmpl_ctx_t ctx, tmpl_cb_t cb)
{
	kv_cb = cb;
	kv_cb->send_default_headers(ctx);
	kv_cb->add_filler_text(ctx, "Hello world!");
	kv_cb->register_element_start_tag(ctx, "kv", &on_kv_start_tag);
	return 0;
}

static void quit() {}

struct templatizer_plugin templatizer_plugin_v1 = {
	&init,
	&quit,
};
