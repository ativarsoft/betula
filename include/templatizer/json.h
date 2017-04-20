#ifndef TMPL_JSON
#define TMPL_JSON

#include <stddef.h>

enum tmpl_json_type {
	TMPL_JSON_STRING,
	TMPL_JSON_NUMBER,
	TMPL_JSON_OBJECT,
	TMPL_JSON_ARRAY,
	TMPL_JSON_BOOLEAN,
	TMPL_JSON_NULL
};

union tmpl_json_data {
	char *string;
	double number;
	struct tmpl_json_object *object;
	struct tmpl_json_array *array;
	int boolean;
};

struct tmpl_json_property {
	char *property;
	enum tmpl_json_type type;
	union tmpl_json_data data;
};

struct tmpl_json_context;

int tmpl_json_feed(struct tmpl_json_context *context, size_t size, void *data);
int tmpl_json_eof(struct tmpl_json_context *context);
struct tmpl_json_context *tmpl_json_create(void);
void tmpl_json_destroy(struct tmpl_json_context *context);

#endif
