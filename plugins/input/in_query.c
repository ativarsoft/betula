/* Copyright (C) 2023 Mateus de Lima Oliveira */

#include <templatizer.h>
#include <templatizer/query.h>
#include <string.h>

static const char mime[] = "application/x-www-form-urlencoded";

int query_callback
    (void *data,
     void *key, size_t keylen,
     void *value, size_t valuelen)
{
	return 0;
}

static int init(tmpl_ctx_t data, tmpl_cb_t cb)
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

const tmpl_plugin_record_t templatizer_plugin_v1 = {
	&init,
	&quit
};
