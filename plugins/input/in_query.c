#include <templatizer.h>
#include <templatizer/query.h>
#include <string.h>

static const char mime[] = "application/x-www-form-urlencoded";

int query_callback(char *s, size_t len)
{
	return 0;
}

static int init(struct context *data, struct templatizer_callbacks *cb)
{
	char *method = getenv("REQUEST_METHOD");
	char *contenttype = getenv("CONTENT_TYPE");
	char *query = getenv("QUERY_STRING");
	if (contenttype == NULL || contenttype == NULL || query == NULL)
		return 0;
	if (strcmp(contenttype, mime) != 0)
		return 0;
	if (strcmp(method, "GET") == 0) {
		tmpl_parse_query_string_get(data, &query_callback);
	}
	return 0;
}

static void quit() {}

struct templatizer_plugin templatizer_plugin_v1 = {
	&init,
	&quit
};
