#ifndef TMPL_TEMPLATIZER_H
#define TMPL_TEMPLATIZER_H

#include <stdlib.h>

#define TMPL_NOT_JMP 0
#define TMPL_JMP 1
#define IF_TRUE TMPL_NOT_JMP
#define IF_FALSE TMPL_JMP
#define SWHILE_TRUE TMPL_NOT_JMP
#define SWHILE_FALSE TMPL_JMP

#define TMPL_CALLBACKS(var) struct templatizer_callbacks *var
#define TMPL_INIT_CALLBACKS(var, cb) var = cb

#define tmpl_assert(err) \
	if (!(err)) { \
		fprintf(stderr, "assertion failed in file %s, function %s, line %d\n", __FILE__, __FUNCTION__, __LINE__); \
		printf("Status: 500\r\n\r\n"); \
		exit(1); \
	}

struct context;
typedef struct context *tmpl_ctx_t;

enum templatizer_compression {
	TMPL_Z_PLAIN,
	TMPL_Z_GZIP,
	TMPL_Z_DEFLATE
};

enum templatizer_format {
	TMPL_FMT_HTML,
	TMPL_FMT_XHTML
};

typedef int (*on_element_callback_t)(tmpl_ctx_t ctx);

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

	int (*add_filler_text)(struct context *data, const char *text);
	int (*add_control_flow)(struct context *data, int b); /* for conditionals */

	int (*register_element_tag)(tmpl_ctx_t ctx, char *s, on_element_callback_t cb);

	void (*exit)(tmpl_ctx_t ctx, int status);
};

struct templatizer_plugin {
	int (*init)(struct context *data, struct templatizer_callbacks *cb);
	void (*quit)();
};

#endif
