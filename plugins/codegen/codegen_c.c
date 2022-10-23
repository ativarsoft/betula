#include <templatizer.h>
#include <stdio.h>

#define QLIST_FOREACH(a, b, c)

struct parameter {
	char *type;
	char *name;
};

static int gen_function_start()
{
	char *func_name = "";
	char *func_ret_type = "";
	struct parameter *arg;
	fputs(func_ret_type, stdout);
	fputc(' ', stdout);
	fputs(func_name, stdout);
	fputc('(', stdout);
	QLIST_FOREACH (arg, arg_list, entry) {
		fputs(arg->type, stdout);
		fputc(' ', stdout);
		fputs(arg->name, stdout);
	}
	fputc(')', stdout);
	return 0;
}

static int gen_function_end()
{
	fputs("}", stdout);
	return 0;
}

static int gen_if_start()
{
	fputs("if", stdout);
	fputc(' ', stdout);
	fputc('{', stdout);
	fputc('\n', stdout);
	return 0;
}

static int gen_if_end()
{
	fputc('}', stdout);
	fputc('\n', stdout);
	return 0;
}

static int gen_swhile_start()
{
	fputs("while", stdout);
	fputc(' ', stdout);
	fputc('{', stdout);
	fputc('\n', stdout);
	return 0;
}

static int gen_ewhile_start()
{
	fputc('}', stdout);
	fputc('\n', stdout);
	return 0;
}

static int init(tmpl_ctx_t data, struct templatizer_callbacks *cb)
{
	/*cb->register_tag_handler
		(data, "if",
		&gen_swhile_start,
		&gen_swhile_end);
	cb->register_tag_handler
		(data, "swhile",
		&gen_swhile_start,
		&gen_swhile_end);
	cb->register_tag_handler
		(data, "ewhile",
		&gen_ewhile_start,
		&gen_ewhile_end);*/
	return 0;
}

static void quit() {}

struct templatizer_plugin templatizer_plugin_v1 = {
	&init,
	&quit
};
