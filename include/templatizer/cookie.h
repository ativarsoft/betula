/* Copyright (C) 2022 Mateus de Lima Oliveira */

#ifndef HTTP_COOKIE_H
#define HTTP_COOKIE_H

#include <stdio.h>

typedef void (*http_cookie_callback_t)
    (void *data,
     const char *key, size_t key_len,
     const char *value, size_t value_len);

int tmpl_parse_cookie_string(void *data, http_cookie_callback_t cb);

#endif
