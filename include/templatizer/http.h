#ifndef TMPL_HTTP
#define TMPL_HTTP

#include <stddef.h>

typedef void (*tmpl_kv_callback)(void *data, char *key, size_t key_len, char *value, size_t value_len);

struct tmpl_key_pair {
	const char *key;
	char *value;
};

struct tmpl_query {
	unsigned int num_params;
	char **keys, **values;
};

int tmpl_parse_query_string_get(void *data, tmpl_kv_callback cb);
int tmpl_parse_query_string_post(void *data, tmpl_kv_callback cb);
int tmpl_parse_cookie_string(void *data, tmpl_kv_callback cb);
void tmpl_throw_error(const char *status, const char *url, const char *msg);
void tmpl_alloc_query(void *query, char *key, int key_len, char *value, int value_len);
void tmpl_free_query(void *query, char *key, int key_len, char *value, int value_len);

#endif
