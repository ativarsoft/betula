/* Copyright (C) 2022 Mateus de Lima Oliveira */

import core.stdc.string : strchr;
import platform;
import stream;

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
        throw new Exception("Invalid environment variable name.");
    }
    char *value = getenv(key.ptr);
    if (value is null) {
        throw new Exception("Environment variable does not exist.");
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

    try {
        query = tmpl_getenv("QUERY_STRING");
        tmpl_parse_query_string(query.ptr, data, cb);
    } catch (Exception e) {
        rc = 1;
    }
    return rc;
}

int tmpl_parse_query_string_post(void *data, http_query_callback_t cb)
{
    string content_length;
    char[] query;
    int len;
    auto freeQuery = () @trusted {
        import core.memory : GC;
        GC.free(cast(void *) query);
    };

    content_length = tmpl_getenv("CONTENT_LENGTH");
    len = tmpl_atoi(content_length);
    query = new char[len + 1];
    query[len] = '\0';
    if (tmpl_fread_stdin(cast(void *) query.ptr, len, 1) != 1) {
        freeQuery();
        return 1;
    }
    tmpl_parse_query_string(query.ptr, data, cb);
    freeQuery();
    return 0;
}
