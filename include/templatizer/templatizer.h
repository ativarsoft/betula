#ifndef TMPL_TEMPLATIZER_H
#define TMPL_TEMPLATIZER_H

#include <stdlib.h>

#define TMPL_NOT_JMP 0
#define TMPL_JMP 1
#define IF_TRUE TMPL_NOT_JMP
#define IF_FALSE TMPL_JMP
#define SWHILE_TRUE TMPL_NOT_JMP
#define SWHILE_FALSE TMPL_JMP

struct context;

typedef void (*kv_callback)(void *data, char *key, size_t key_len, char *value, size_t value_len);

enum templatizer_compression {
	TMPL_Z_PLAIN,
	TMPL_Z_GZIP,
	TMPL_Z_DEFLATE
};

enum templatizer_format {
	TMPL_FMT_HTML,
	TMPL_FMT_XHTML
};

struct templatizer_callbacks {
	/* Templatizer will manage the memory for the plugin. */
	/* This avoids memory leaks. */
	/* This is useful especially if Templatizer is run as a web server module. */
	void *(*malloc)(struct context *data, size_t size);
	void (*free)(struct context *data, void *ptr);

	void (*set_compression)(struct context *data, enum templatizer_compression opt);
	void (*set_keep_alive)(struct context *data);
	void (*send_header)(struct context *data, const char *key, const char *value);
	void (*send_default_headers)(struct context *data);
	void (*set_output_format)(struct context *data, enum templatizer_format fmt);

	int (*add_filler_text)(struct context *data, char *text);
	int (*add_control_flow)(struct context *data, int b); /* for conditionals */
};

struct templatizer_plugin {
	int (*init)(struct context *data, struct templatizer_callbacks *cb);
	void (*quit)();
};

int parse_query_string_get(void *data, kv_callback cb);
int parse_query_string_post(void *data, kv_callback cb);
int parse_cookie_string(void *data, kv_callback cb);

#endif
