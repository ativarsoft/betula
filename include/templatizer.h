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
		fputs("Status: 500 Internal Server Error\r\n", stdout); \
		fputs("Content-Type: text/plain\r\n", stdout); \
		fputs("\r\n", stdout); \
		fputs("Internal Server Error\n", stdout); \
		fprintf(stdout, "assertion failed in file %s, function %s, line %d\n", __FILE__, __FUNCTION__, __LINE__); \
		exit(0); \
	}

#define TMPL_ASSERT(err) tmpl_assert(err)

#define TMPL_ADD(a, b, c) \
{ \
    if (__builtin_add_overflow(a, b, &c)) { \
        fprintf(stderr, "%s:%d: integer overflow.\n", __FILE__, __LINE__); \
        printf("Status: 500 Internal Server Error\r\n"); \
        printf("Content-Type: text/plain\r\n"); \
        printf("\r\n"); \
        printf("Integer overflow.\n"); \
        exit(0); \
    } \
}

#define TMPL_ASSIGN_PTR(lhs, rhs) \
{ \
    if (lhs) { \
        *lhs = rhs; \
    } else { \
        fprintf(stderr, "%s:%d: null pointer exception.\n", __FILE__, __LINE__); \
        printf("Status: 500 Internal Server Error\r\n"); \
        printf("Content-Type: text/plain\r\n"); \
        printf("\r\n"); \
        printf("Null pointer exception.\n"); \
        exit(0); \
    } \
}

#define TMPL_ASSIGN(lhs, rhs) \
{ \
    (lhs) = (rhs); \
    if ((lhs) != (rhs)) { \
        fprintf(stderr, "%s:%d: assignment error.\n", __FILE__, __LINE__); \
        printf("Status: 500 Internal Server Error\r\n"); \
        printf("Content-Type: text/plain\r\n"); \
        printf("\r\n"); \
        printf("Assignment error.\n"); \
        exit(0); \
    } \
}

#ifndef ADD
#define ADD(a, b, c) TMPL_ADD(a, b, c)
#endif

#ifndef ASSIGN_PTR
#define ASSIGN_PTR(lhs, rhs) TMPL_ASSIGN_PTR(lhs, rhs)
#endif

#ifndef ASSIGN
#define ASSIGN(lhs, rhs) TMPL_ASSIGN(lhs, rhs)
#endif

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
