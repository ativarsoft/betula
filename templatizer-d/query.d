/* Copyright (C) 2022 Mateus de Lima Oliveira */

module query;

import core.stdc.string : strchr;
import platform;
import stream;

extern(C) {

alias http_query_callback_t = int function
    (void *data,
     void *key, size_t key_len,
     void *value, size_t value_len);

int tmpl_parse_query_string_get(void *data, http_query_callback_t cb);
int tmpl_parse_query_string_post(void *data, http_query_callback_t cb);

static string tmpl_getenv(string key)
{
    import core.stdc.stdlib : getenv;
    if (key is null) {
        return null;
    }
    char *value = getenv(key.ptr);
    if (value is null) {
        return null;
    }
    return cast(string) value[0..strlen(value)];
}

void tmpl_parse_query_string(const(char) *query, void *data, http_query_callback_t cb)
{
    const(char) *p;
    const(char) *token;
    const(char) *key;
    const(char) *value;
    size_t key_len, value_len;

    p = query;
    for (;;) {
        /* get key's length */
        token = strchr(p, '=');
        if (token == null)
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
             cb(data, cast(void *) key, key_len, cast(void *) value, value_len);
        } else {
             /* This is the last value. */
             value_len = strlen(p);
             value = p;
             cb(data, cast(void *) key, key_len, cast(void *) value, value_len);
             break;
        }
        p += value_len + 1;
    }
}

int tmpl_parse_query_string_get(void *data, http_query_callback_t cb)
{
    string query;
    int rc = 0;

    query = tmpl_getenv("QUERY_STRING");
    if (query is null) {
        rc = 1;
        goto _out;
    }
    tmpl_parse_query_string(query.ptr, data, cb);
_out:
    return rc;
}

static char[] create_query_buffer(size_t len) @trusted
{
    import core.stdc.stdlib : calloc;
    char *buffer = cast(char *) calloc(char.sizeof, len);
    return buffer[0..char.sizeof * len];
}

static void free_query_buffer(char[] buffer) @trusted
{
    import core.stdc.stdlib : free;
    free(buffer.ptr);
    buffer.destroy();
}

int tmpl_parse_query_string_post(void *data, http_query_callback_t cb)
{
    string content_length;
    char[] query;
    int len;

    content_length = tmpl_getenv("CONTENT_LENGTH");
    len = tmpl_atoi(content_length);
    query = create_query_buffer(len + 1);
    scope(exit) free_query_buffer(query);
    query[len] = '\0';
    if (tmpl_fread_stdin(cast(void *) query.ptr, len, 1) != 1) {
        return 1;
    }
    tmpl_parse_query_string(query.ptr, data, cb);
    return 0;
}

} /* extern(C) */
