/* Copyright (C) 2017-2023 Mateus de Lima Oliveira */

#ifndef TMPL_TEMPLATIZER_H
#define TMPL_TEMPLATIZER_H

#include <stdlib.h>

#define STORAGE_LMDB

#ifdef STORAGE_LMDB
#include <lmdb.h>
#endif

#define TMPL_NOT_JMP 0
#define TMPL_JMP 1
#define IF_TRUE TMPL_TRUE
#define IF_FALSE TMPL_FALSE
#define SWHILE_TRUE TMPL_TRUE
#define SWHILE_FALSE TMPL_FALSE
#define TMPL_TRUE 1
#define TMPL_FALSE 0

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

#if 0
#ifndef ASSIGN_PTR
#define ASSIGN_PTR(lhs, rhs) TMPL_ASSIGN_PTR(lhs, rhs)
#endif

#ifndef ASSIGN
#define ASSIGN(lhs, rhs) TMPL_ASSIGN(lhs, rhs)
#endif
#endif

/* Safe pointer dereference that checks for null pointers.
 * TMPL_DEREF also works as a l-value. */
#define TMPL_DEREF(x) (*((x) ? (x) : (exit(1),(x)))

struct context;
typedef struct context *tmpl_ctx_t;
typedef struct pollen_callbacks *tmpl_cb_t;
typedef const char **tmpl_attr_t;

#ifdef STORAGE_LMDB
typedef MDB_txn *tmpl_txn_t;
typedef MDB_dbi tmpl_dbi_t;
#endif

typedef void *tmpl_sql_t;

enum templatizer_compression {
	TMPL_Z_PLAIN,
	TMPL_Z_GZIP,
	TMPL_Z_DEFLATE
};

enum templatizer_format {
	TMPL_FMT_HTML,
	TMPL_FMT_XHTML
};

typedef int (*on_element_start_callback_t)
    (tmpl_ctx_t ctx, const char *el, const char **attr);
typedef int (*on_element_end_callback_t)
    (tmpl_ctx_t ctx, const char *el);
typedef int (*tmpl_codegen_tag_start_t)
    (tmpl_ctx_t ctx,
     const char *el,
     const char **attr);
typedef int (*tmpl_codegen_tag_end_t)
    (tmpl_ctx_t ctx,
     const char *el);
typedef int (*tmpl_codegen_control_flow_start_t)
    (tmpl_ctx_t ctx);
typedef int (*tmpl_codegen_control_flow_end_t)
    (tmpl_ctx_t ctx,
     const char *label);

/* Templatizer callbacks emulate system calls. */
struct pollen_callbacks {
	/* Templatizer will manage the memory for the plugin. */
	/* This avoids memory leaks. */
	/* This is useful especially if Templatizer is run as a web server module. */
	void *(*malloc)(struct context *data, size_t size);
	void (*free)(struct context *data, void *ptr);

	char *(*strndup)(tmpl_ctx_t ctx, const char *ptr, size_t length);

	/*
	 * Link-time functions
	 */

	/*
	 * Runtime functions:
	 * These are used by the callbacks inside the nodes.
	 * They are specific to the standard I/O streams.
	 */
	void (*set_compression)(struct context *data, enum templatizer_compression opt);
	void (*set_keep_alive)(struct context *data);
	void (*send_header)(struct context *data, const char *key, const char *value);
	void (*send_default_headers)(struct context *data);
	void (*set_output_format)(struct context *data, enum templatizer_format fmt);

	int (*add_filler_text)(struct context *data, const char *text);
	int (*add_control_flow)(struct context *data, int b); /* for conditionals */

	/*
	 * Compile-time functions:
	 * These can be used to generate a parse tree.
	 */

	/* These are called during parse time. */
	/* They should be used to generate nodes. */
	int (*register_element_start_tag)(tmpl_ctx_t ctx, const char *s, on_element_start_callback_t cb);
	int (*register_element_end_tag)(tmpl_ctx_t ctx, const char *s, on_element_end_callback_t cb);

	int (*new_attr_object)(int num);
	int (*set_attr_value)(const char *key, const char *value);

	int (*add_start_node)(tmpl_ctx_t ctx, const char *el, tmpl_attr_t attr);
	int (*add_end_node)(tmpl_ctx_t ctx, const char *el);
	int (*add_selfclosing_html_node)(tmpl_ctx_t ctx, const char *el, tmpl_attr_t attr);
	int (*add_if_node)(tmpl_ctx_t ctx);
	int (*add_swhile_node)(tmpl_ctx_t ctx);
	int (*add_ewhile_node)(tmpl_ctx_t ctx);
	int (*add_call_special_node)(tmpl_ctx_t ctx, void (*f)());

	int (*codegen_sanity_check)();

	void (*noplugin)();

	void (*exit)(tmpl_ctx_t ctx, int status);
	int (*get_num_plugin_parameters)(tmpl_ctx_t ctx);
	int (*get_plugin_parameter)(tmpl_ctx_t ctx, int index, const char **param_ptr, size_t *param_length);

	const char *(*get_version_string)();
	const char *(*get_copyright_string)();
	int (*get_int_variable)(tmpl_ctx_t ctx, const char *name);
	int (*set_int_variable)(tmpl_ctx_t ctx, const char *name, int value);

	int (*regex_compile)();
	int (*regex_execute)();
	int (*regex_cleanup)();

	/*
	 * I/O functions:
	 *
	 * Library agnostic API.
	 * This API can be used to make plugin
	 * code portable among different
	 * operatimg systems and plugin
	 * languages. The only system dependent
	 * code being the Templatizer plugin host
	 * executable and the language runtime
	 * of the plugin, if any. */

	/* Storage functions are key-value database functions
	 * for emulating disk storage. */
	int (*storage_open)(const char *path);
	int (*storage_begin_transaction)
	  (tmpl_txn_t *txn);
	int (*storage_commit_transaction)
	  (tmpl_txn_t txn);
	int (*storage_open_database)
	  (tmpl_txn_t txn, tmpl_dbi_t *dbi);
	int (*storage_close_database)
	  (tmpl_dbi_t dbi);
	int (*storage_get_string)
	  (tmpl_txn_t txn,
	   tmpl_dbi_t dbi,
	   int key_id,
	   char **value);
	int (*storage_get_integer)
	  (tmpl_txn_t txn,
	   tmpl_dbi_t dbi,
	   int key_id,
	   int *value);
	int (*sql_connect)(const char *s, tmpl_sql_t **connection);
	int (*sql_disconnect)(tmpl_sql_t *connection);
	int (*sql_execute)(tmpl_sql_t *connection);
	int (*sql_prepare)();
	int (*vm_define)();
	int (*vm_start)(const char *name);
	int (*vm_destroy)(const char *name);
};

struct pollen_plugin {
	int (*init)(tmpl_ctx_t data, tmpl_cb_t cb);
	void (*quit)();
};

typedef struct pollen_plugin *tmpl_plugin_t, tmpl_plugin_record_t;

#endif
