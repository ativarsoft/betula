#include "templatizer.h"

TMPL_CALLBACKS(tmpl);

int string_templatizer;

static void print_start(tmpl_ctx_t ctx, int el, int attr)
{
	int placeholder = 0;
        int key = 0;
	int value = 0;

	placeholder = tmpl->get_placeholder_string(ctx);
	tmpl->putchar(ctx, '<');
	tmpl->print(ctx, el);
	if (!tmpl->is_null(ctx, attr)) {
		tmpl->putchar(ctx, ' ');
		tmpl->print(ctx, key);
		tmpl->putchar(ctx, '=');
		tmpl->putchar(ctx, '\"');
		if (tmpl->strcmp(placeholder, value) == 0)
			tmpl->dump_string(ctx);
		else
			tmpl->print(ctx, value);
		tmpl->putchar(ctx, '\"');
	}
	tmpl->putchar(ctx, '>');
	tmpl->exit(ctx, 0);
	return 0;
}

int on_p_tag(tmpl_ctx_t ctx, int el, int attr)
{
	/*int paragraph = tmpl->create_string("<p>");
	tmpl->print(ctx, paragraph);*/
	print_start(ctx, el, attr);
	return 0;
}

static int init(tmpl_ctx_t ctx, struct templatizer_callbacks *cb)
{
	TMPL_INIT_CALLBACKS(tmpl, cb);
	tmpl->register_element_tag(ctx, "p", &on_p_tag);
	return 0;
}

static void quit() {}

struct templatizer_plugin templatizer_plugin_v1 = {
	&init,
	&quit,
};
