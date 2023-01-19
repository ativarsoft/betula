#include "templatizer.h"
#include <threads.h>

TMPL_CALLBACKS(tmpl);

thread_local tmpl_cb_t html_cb = NULL;

int on_selfclosing_html_start_tag(tmpl_ctx_t ctx, const char *el, tmpl_attr_t attr)
{
	int rc = -1;
	/*rc = html_cb->add_selfclosing_html_node(ctx, el, attr);
	if (rc != 0)
		return 1;*/
	return 0;
}

int on_selfclosing_html_end_tag(tmpl_ctx_t ctx, const char *el)
{
	return 0;
}

static int init(tmpl_ctx_t ctx, struct templatizer_callbacks *cb)
{
	int rc = -1;
	html_cb = cb;
	rc = cb->register_element_start_tag(ctx, "meta", &on_selfclosing_html_start_tag);
	if (rc != 0)
		return 1;
	rc = cb->register_element_start_tag(ctx, "br", &on_selfclosing_html_start_tag);
	if (rc != 0)
		return 1;
	rc = cb->register_element_end_tag(ctx, "meta", on_selfclosing_html_end_tag);
	if (rc != 0)
		return 1;
	rc = cb->register_element_end_tag(ctx, "br", on_selfclosing_html_end_tag);
	if (rc != 0)
		return 1;
	return 0;
}

static void quit() {}

struct templatizer_plugin templatizer_plugin_v1 = {
	&init,
	&quit,
};
