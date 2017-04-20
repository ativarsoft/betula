#include <stdio.h>
#include <templatizer/json.h>

const char data[] = "{\"number\": 1234.5678}";

int main()
{
	struct tmpl_json_context *context;

	context = tmpl_context_create();
	if (context == NULL)
		return 1;
	tmpl_json_feed(context, strlen(data), data);
	tmpl_json_eof(context);
	tmpl_context_destroy(context);
	return 0;
}
