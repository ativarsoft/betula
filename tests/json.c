#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <templatizer/templatizer.h>

const char text[] = "{\"number\": 1}";
struct data {
	double x;
} bin;
const struct tmpl_json_member members[] = {
	{TMPL_JSON_NUMBER, "number", offsetof(struct data, x)}
};
struct tmpl_json_object root = {
	(struct tmpl_json_member *) members,
	&bin
};

int main()
{
	struct tmpl_json_context *context;

	context = tmpl_json_create();
	if (context == NULL)
		return 1;
	tmpl_json_feed(context, strlen(text), text);
	tmpl_json_eof(context);
	tmpl_json_destroy(context);
	return 0;
}
