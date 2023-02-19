import templatizer;
import core.stdc.string : strcmp;

__gshared tmpl_cb_t cb;

extern(C)
@system
int pollen_codegen_sanity_check();

static int codegen_sanity_checks()
@trusted
{
	return cb.codegen_sanity_check();
}

static string toString(const(char) *s)
@trusted
{
	import core.stdc.string : strlen;
	return cast(string) s[0..strlen(s)];
}

static void add_filler_text(tmpl_ctx_t ctx, string s)
@trusted
{
	cb.add_filler_text(ctx, s.ptr, s.length);
}

static int add_filler_text_expr(tmpl_ctx_t ctx, const(char) *sanity_check_test)
@safe
{
	string sctstr = toString(sanity_check_test);
	if (sctstr == "codegen") {
		if (codegen_sanity_checks() == 0) {
			add_filler_text(ctx, "OK");
		} else {
			add_filler_text(ctx, "Fail");
		}
	} else {
		add_filler_text(ctx, "Unknown test!");
	}
	return 0;
}

static int sanityparseattr(tmpl_ctx_t ctx, const(char) **attr)
{
	for (int i = 0; attr[i]; i+=2) {
		string s = toString(attr[i]);
		if (s == "test") {
			add_filler_text_expr(ctx, attr[i+1]);
		}
	}
	return 0;
}

extern(C)
static int sanitystartcb(tmpl_ctx_t ctx, const(char) *el, const(char) **attr)
{
	string s = toString(el);
	if (s == "sanity") {
		sanityparseattr(ctx, attr);
	}
	return 0;
}

extern(C)
static int sanityendcb(tmpl_ctx_t ctx, const(char) *el)
{
	/* Nothing to be done. */
	return 0;
}

extern(C)
static int init(tmpl_ctx_t data, tmpl_cb_t _cb)
{
	cb = _cb;
	cb.register_element_start_tag(data, "sanity", &sanitystartcb);
	cb.register_element_end_tag(data, "sanity", &sanityendcb);
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
