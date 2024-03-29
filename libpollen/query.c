/* Copyright (C) 2022 Mateus de Lima Oliveira */
#include <templatizer/query.h>
#include <stdio.h>
#include <string.h> /* strchr() */
#include <stdlib.h> /* getenv() */

void tmpl_parse_query_string(char *query, void *data, http_query_callback_t cb)
{
	char *p, *token;
	char *key, *value;
	size_t key_len, value_len;

	p = query;
	for (;;) {
		/* get key's length */
		token = strchr(p, '=');
		if (token == NULL)
			break; /* No key after amp or key has no value. */
		key = p;
		key_len = token - p;
		p += key_len + 1;

		/* get value's length */
		token = strchr(p, '&');
		if (token) {
			/* This is NOT the last value. */
			value_len = token - p;
			value = p;
			cb(data, key, key_len, value, value_len);
		} else {
			/* This is the last value. */
			value_len = strlen(p);
			value = p;
			cb(data, key, key_len, value, value_len);
			break;
		}
		p += value_len + 1;
	}
}

int tmpl_parse_query_string_get(void *data, http_query_callback_t cb)
{
	char *query;

	query = getenv("QUERY_STRING");
	if (query == NULL)
		return 1;
	tmpl_parse_query_string(query, data, cb);
	return 0;
}

int tmpl_parse_query_string_post(void *data, http_query_callback_t cb)
{
	char *content_length, *query;
	int len;

	content_length = getenv("CONTENT_LENGTH");
	if (content_length == NULL)
		return 1;
	len = atoi(content_length);
	query = calloc(sizeof(char), len + 1);
	if (query == NULL)
		return 1;
	query[len] = '\0';
	if (fread(query, len, 1, stdin) != 1) {
		free(query);
		return 1;
	}
	tmpl_parse_query_string(query, data, cb);
	free(query);
	return 0;
}
