#ifndef HTTP_QUERY_H
#define HTTP_QUERY_H

typedef void (*http_query_callback_t)
    (void *data,
     void *key, size_t key_len,
     void *value, size_t value_len);

void tmpl_parse_query_string
    (char *query, void *data, http_query_callback_t cb);

int tmpl_parse_query_string_get(void *data, http_query_callback_t cb);
int tmpl_parse_query_string_post(void *data, http_query_callback_t cb);

#endif
