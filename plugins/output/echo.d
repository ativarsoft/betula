import templatizer;
import core.stdc.string : strcmp;

__gshared tmpl_cb_t cb;

static int add_filler_text_expr(tmpl_ctx_t ctx, const(char) *identifier)
{
	const(char) *ptr;
	size_t length;
	//cb->get_variable_string(ctx, identifier, &ptr, &length);
	//cb.add_filler_text(ctx, ptr, length);
	cb.add_filler_text(ctx, identifier, 0);
	return 0;
}

static int echoparseattr(tmpl_ctx_t ctx, const(char) **attr)
{
	for (int i = 0; attr[i]; i+=2) {
		if (strcmp(attr[i], "line") == 0) {
			add_filler_text_expr(ctx, attr[i+1]);
		}
	}
	return 0;
}

extern(C)
static int echostartcb(tmpl_ctx_t ctx, const(char) *el, const(char) **attr)
{
	if (strcmp(el, "echo") == 0) {
		echoparseattr(ctx, attr);
	}
	return 0;
}

extern(C)
static int echoendcb(tmpl_ctx_t ctx, const(char) *el)
{
	/* Nothing to be done. */
	return 0;
}

extern(C)
static int init(tmpl_ctx_t data, tmpl_cb_t _cb)
{
	cb = _cb;
	cb.register_element_start_tag(data, "echo", &echostartcb);
	cb.register_element_end_tag(data, "echo", &echoendcb);
	return 0;
}

extern(C)
static void quit() {}

extern(C)
__gshared
immutable(templatizer_plugin) templatizer_plugin_v1 = {
	&init,
	&quit
};
