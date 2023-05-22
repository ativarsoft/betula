/* Copyright (C) 2023 Mateus de Lima Oliveira */

/* Simple interpreted math intrinsics. */

#include <pollen/pollen.h>
#include <string.h>
#include <stdio.h>
#include "math-rs/math_rs.h"

#define BUFFER_SIZE 32

static int parse(char *str, size_t len)
{
	int result = 0;
	int rc = -1;

	rc = calc_three_address_code("add", 1, 1, &result);
	if (rc != 0)
		return 1;
	if (result != 2)
		return 2;
	len = snprintf(str, BUFFER_SIZE, "%.31d", result);
	if (len >= BUFFER_SIZE)
		return 3;
	return 0;
}

static int init(tmpl_ctx_t data, tmpl_cb_t cb)
{
	char str[BUFFER_SIZE] = {0};
	if (parse(str, BUFFER_SIZE) != 0)
		return 1;
	cb->send_default_headers(data);
	cb->add_filler_text(data, str);
	return 0;
}

static void quit() {}

const tmpl_plugin_record_t templatizer_plugin_v1 = {
	&init,
	&quit
};
