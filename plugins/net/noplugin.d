import templatizer;

__gshared tmpl_cb_t cb;

static string toString(const(char) *s)
@trusted
{
	import core.stdc.string : strlen;
	return cast(string) s[0..strlen(s)];
}

static void noplugin(tmpl_ctx_t ctx)
@trusted
{
	cb.noplugin();
}

static int disable_plugins(tmpl_ctx_t ctx)
@safe
{
	noplugin(ctx);
	return 0;
}

extern(C)
static int nopluginstartcb(tmpl_ctx_t ctx, const(char) *el, const(char) **attr)
{
	string s = toString(el);
	if (s == "noplugin") {
		disable_plugins(ctx);
	}
	return 0;
}

extern(C)
static int nopluginendcb(tmpl_ctx_t ctx, const(char) *el)
{
	/* Nothing to be done. */
	return 0;
}

extern(C)
static int init(tmpl_ctx_t data, tmpl_cb_t _cb)
{
	cb = _cb;
	cb.register_element_start_tag(data, "noplugin", &nopluginstartcb);
	cb.register_element_end_tag(data, "noplugin", &nopluginendcb);
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
