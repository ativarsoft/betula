/* Copyright (C) 2017-2023 Mateus de Lima Oliveira */

#include <unistd.h>
#include <libgen.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h> /* atoi() */
#include <string.h>
#include <ctype.h>
#include <sys/queue.h>
#include <expat.h>
#include <templatizer.h>
#include <ctype.h>
#include <apr_pools.h>
#include <apr_strings.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <templatizer/compiler/compiler.h>
#include <assert.h>
#include "memory.h"
#include "interpreter.h"
#include "opcode.h"
#include "storage.h"
#include "virt.h"
#include "config.h"
#include "tmplregex.h"
#include "jit.h"

#define VERSION "0.1"
#define COPYRIGHT "Copyright (C) 2017-2023 Mateus de Lima Oliveira"

static int parse_xml_file(struct context *data, const char *tmpl);

static const char tmpl_version[] = VERSION;
static const char tmpl_copyright[] = COPYRIGHT;

enum {
    ELEMENT_HANDLED = 0,
    ELEMENT_NOT_HANDLED = 1
};

/* Function prototypes */
static struct node *add_node(struct context *data);
static FILE *open_path_translated(tmpl_ctx_t data, const char *pathtranslated);
static const char *tmpl_get_version_string();
static int tmpl_try_get_int_variable
  (tmpl_ctx_t ctx,
   const char *name,
   int *value);
static int tmpl_add_selfclosing_html_node(tmpl_ctx_t ctx, const char *el, tmpl_attr_t attr);

#define TMPL_MASK_BITS(bits) (~0L << (bits))
#define TMPL_MASK(type) TMPL_MASK_BITS(sizeof(type) * 8)

#ifndef TMPL_CAST
#define TMPL_CAST(value, type) ((value | TMPL_MASK(type)) == value)? (type) value : abort())
#endif

static struct context *get_tmpl_context(tmpl_ctx_t ctx)
{
	return (struct context *) ctx;
}

void *templatizer_malloc(tmpl_ctx_t data, size_t size)
{
#if 1
	return apr_pcalloc(data->pools.process, size);
#else
	return malloc(size);
#endif
}

void templatizer_free(tmpl_ctx_t data, void *ptr)
{
	(void) data;
	(void) ptr;
	//free(ptr);
}

static char *templatizer_strdup(tmpl_ctx_t data, const char *s)
{
	return apr_pstrdup(data->pools.process, s);
}

static char *tmpl_process_strdup(tmpl_ctx_t data, const char *s)
{
	return apr_pstrdup(data->pools.process, s);
}

static char *tmpl_connection_strndup(tmpl_ctx_t ctx, const char *ptr, size_t length)
{
	return apr_pstrndup(ctx->pools.connection, ptr, length);
}

void *tmpl_process_malloc(ctx, size)
tmpl_ctx_t ctx;
size_t size;
{
	return templatizer_malloc(ctx, size);
}

/* Data allocated by this function is freed when
 * connection is closed. */
void *tmpl_connection_malloc(tmpl_ctx_t ctx, size_t size)
{
#if 1
	return apr_pcalloc(ctx->pools.connection, size);
#else
	return malloc(size);
#endif
}

void set_compression(struct context *data, enum templatizer_compression opt)
{
	data->output_compression = opt;
}

void set_keep_alive(struct context *data)
{
	data->keep_alive = true;
}

void send_header(struct context *data, const char *key, const char *value)
{
	(void) data;
	if (key)
		printf("%s: %s\r\n", key, value);
	else
		fputs("\r\n", stdout);
}

void send_default_headers(struct context *data)
{
	const char *content_type;

	switch (data->output_format) {
		case TMPL_FMT_HTML:
		content_type = "text/html; charset=utf-8";
		break;
		case TMPL_FMT_XHTML:
		content_type = "application/xhtml+xml; charset=utf-8";
		break;
	}
	send_header(data, "Content-Type", content_type);
	send_header(data, "X-Powered-By", tmpl_get_version_string());
	send_header(data, NULL, NULL);
}

void set_output_format(struct context *data, enum templatizer_format fmt)
{
	data->output_format = fmt;
}

static struct input *add_input(struct context *data)
{
	struct input *n;

	if ((n = templatizer_malloc(data, sizeof(struct input))) == NULL)
		goto nomem;
	TAILQ_INSERT_TAIL(data->input, n, entries);
	return n;
nomem:
	fprintf(stderr, "not enough memory for new input\n");
	XML_StopParser(data->parser, 0);
	return NULL;
}

static int add_filler_text(struct context *data, const char *text)
{
	struct input *p;

	p = add_input(data);
	if ((p->data.filler_text = apr_pstrdup(data->pools.process, text)) == NULL)
		return 1;
	p->type = INPUT_FILLER_TEXT;
	return 0;
}

static int add_control_flow(struct context *data, int b)
{
	struct input *p;

	if ((p = add_input(data)) == NULL)
		return 1;
	p->data.control_flow = (bool) b;
	p->type = INPUT_CONTROL_FLOW;
	return 0;
}

static bool is_control_flow_keyword(const XML_Char *el)
{
        if (strcmp("if", el) == 0
                || strcmp("swhile", el) == 0
                || strcmp("ewhile", el) == 0)
                return true;
        return false;
}

static int tmpl_register_element_start_tag(tmpl_ctx_t ctx, const char *s, on_element_start_callback_t cb)
{
	struct context *data = get_tmpl_context(ctx);
	struct element_start_callback *p;
	p = calloc(1, sizeof(struct element_start_callback));
	if (p == NULL)
		return 1;
	p->el = tmpl_process_strdup(ctx, s);
	p->f = cb;
	TAILQ_INSERT_TAIL(data->on_element_start_callbacks, p, entries);
	return 0;
}

static int tmpl_register_element_end_tag(tmpl_ctx_t ctx, const char *s, on_element_end_callback_t cb)
{
	struct context *data = get_tmpl_context(ctx);
	struct element_end_callback *p;
	p = calloc(1, sizeof(struct element_end_callback));
	if (p == NULL)
		return 1;
	p->el = tmpl_process_strdup(ctx, s);
	p->f = cb;
	TAILQ_INSERT_TAIL(data->on_element_end_callbacks, p, entries);
	return 0;
}

static void tmpl_exit(tmpl_ctx_t ctx, int status)
{
	struct context *data = get_tmpl_context(ctx);
	data->status = status;
}

int tmpl_get_num_plugin_parameters(tmpl_ctx_t ctx)
{
	struct plugin_parameters_head *head = &ctx->plugin_parameters;
	struct plugin_parameters *np;
	int i = 0;
	TAILQ_FOREACH(np, head, entries)
		i++;
	return i++;
}

/* Get plugin parameter supplied on the teplatizer tag. */
int tmpl_get_plugin_parameter(tmpl_ctx_t ctx, int index, const char **param_ptr, size_t *param_length)
{
	struct plugin_parameters_head *head = &ctx->plugin_parameters;
	struct plugin_parameters *np;
	int i = 0;
	int rc = 1;
	TAILQ_FOREACH(np, head, entries) {
		if (i == index) {
			*param_ptr = np->param_ptr;
			*param_length = np->param_length;
			rc = 0;
			break;
		}
		i++;
	}
	return rc;
}

enum plugin_variable_type {
	PLUGIN_VAR_TYPE_INT
};

struct plugin_variable {
	TAILQ_ENTRY(plugin_variable) entries;
	char *name;
	enum plugin_variable_type type_id;
	union {
		int l;
		char *s;
	} value;
};

TAILQ_HEAD(plugin_variable_list_head, plugin_variable);

static int tmpl_try_get_int_variable(ctx, name, value)
tmpl_ctx_t ctx;
const char *name;
int *value;
{
	struct plugin_variable *np = NULL;
	struct plugin_variable_list_head *head =
		ctx->plugin_variables;
	int l;

	*value = 0;

	TAILQ_FOREACH(np, head, entries) {
		if (strcmp(name, np->name) == 0 &&
		    np->type_id == PLUGIN_VAR_TYPE_INT) {
			l = np->value.l;
			*value = l;
			return 0;
		}
	}
	return 1;
}

/* This function returns zero if the variable value
 * is undefined. */
static int tmpl_get_int_variable(ctx, name)
tmpl_ctx_t ctx;
const char *name;
{
	int l;
	int rc;

	rc = tmpl_try_get_int_variable(ctx, name, &l);
	if (rc != 0) {
		return 0;
	}
	return l;
}

static int tmpl_set_int_variable(ctx, name, value)
tmpl_ctx_t ctx;
const char *name;
int value;
{
	struct plugin_variable *np = NULL;
	struct plugin_variable_list_head *head =
		ctx->plugin_variables;
	int rc = -1;
	int tmp;

	rc = tmpl_try_get_int_variable(ctx, name, &tmp);
	if (rc == 0) {
		/* Variable already exists */
		return 1;
	}
	np = tmpl_connection_malloc(ctx, sizeof(*np));
	if (np == NULL) {
		return 2;
	}
	np->name = tmpl_process_strdup(ctx, name);
	np->type_id = PLUGIN_VAR_TYPE_INT;
	np->value.l = value;
	TAILQ_INSERT_TAIL(head, np, entries);
	return 0;
}

static const char *tmpl_get_version_string()
{
	return tmpl_version;
}

static const char *tmpl_get_copyright_string()
{
	return tmpl_copyright;
}

static struct templatizer_callbacks callbacks = {
	.malloc = &templatizer_malloc,
	.free = &templatizer_free,
	.strndup = &tmpl_connection_strndup,
	.set_compression = &set_compression,
	.set_keep_alive = &set_keep_alive,
	.send_header = &send_header,
	.send_default_headers = &send_default_headers,
	.set_output_format = &set_output_format,
	.add_filler_text = &add_filler_text,
	.add_control_flow = &add_control_flow,
	.register_element_start_tag = &tmpl_register_element_start_tag,
	.register_element_end_tag = &tmpl_register_element_end_tag,
	.add_selfclosing_html_node = &tmpl_add_selfclosing_html_node,
	.codegen_sanity_check = &pollen_codegen_sanity_check,
	.exit = &tmpl_exit,
	.get_num_plugin_parameters = &tmpl_get_num_plugin_parameters,
	.get_plugin_parameter = &tmpl_get_plugin_parameter,
	.get_version_string = &tmpl_get_version_string,
	.get_copyright_string = &tmpl_get_copyright_string,
	.get_int_variable = &tmpl_get_int_variable,
	.set_int_variable = &tmpl_set_int_variable,
	.regex_compile = &tmpl_regex_compile,
#ifdef USE_STORAGE
	.storage_open = &storage_open,
        .storage_begin_transaction = &storage_begin_transaction,
        .storage_commit_transaction = &storage_commit_transaction,
        .storage_open_database = &storage_open_database,
	.storage_close_database = &storage_close_database,
        .storage_get_string = &storage_get_string,
	.storage_get_integer = &storage_get_integer,
#endif
#ifdef USE_VIRTUALIZATION
	.vm_define = &vmDefine,
	.vm_start = &vmStart,
	.vm_destroy = &vmDestroy
#endif
};

#ifdef _WIN32
#error win32 is not supported yet
#else
static void *load_library(struct context *data, const char *path)
{
	void *handle;
	const char *path_translated, *dir, *full_path;

	path_translated = data->script_path;
	if (path_translated == NULL) return NULL;
	path_translated = apr_pstrdup(data->pools.process, path_translated);
	if (path_translated == NULL) return NULL;
	dir = dirname(path_translated);
	full_path = templatizer_malloc(data, strlen(dir) + 1 + strlen(path) + 1);
	if (full_path == NULL) {
		templatizer_free(data, (void *) path_translated);
		return NULL;
	}
	sprintf((char *) full_path, "%s/%s", dir, path);
	templatizer_free(data, (void *) path_translated);
	handle = dlopen(full_path, RTLD_LAZY);
	if (handle == NULL) {
		fprintf(stderr, "%s\n", dlerror());
		templatizer_free(data, (void *) full_path);
		return NULL;
	}
	data->plugin_data = dlsym(handle, "templatizer_plugin_v1");
	if (data->plugin_data == NULL) {
		dlclose(handle);
		fprintf(stderr, "%s\n", dlerror());
		templatizer_free(data, (void *) full_path);
		return NULL;
	}
	templatizer_free(data, (void *) full_path);
	data->plugin_handle = handle;
	return handle;
}

static void unload_library(struct context *data)
{
	dlclose(data->plugin_handle);
}

static void *get_symbol(struct context *data, const char *name)
{
	return dlsym(data->plugin_handle, name);
}
#endif

static bool parse_templatizer_tag(struct context *data, const XML_Char **attr)
{
	int i;

	for (i = 0; attr[i]; i += 2) {
		if (strcmp("lib", attr[i]) == 0) {
			if ((data->plugin_handle = load_library(data, attr[i + 1])) == NULL) {
				fprintf(stderr, "unable to load library '%s'\n", attr[i + 1]);
				return false;
			}
			if (data->plugin_data->init(data, &callbacks) != 0)
				return false;
		}
	}
	return true;
}

static int parse_include_tag(struct context *data, const XML_Char **attr)
{
	int i;
	char *path_translated, *dir, *full_path;

	for (i = 0; attr[i]; i += 2) {
		if (strcmp("file", attr[i]) == 0) {
			path_translated = getenv("PATH_TRANSLATED");
			if (path_translated == NULL) return ELEMENT_NOT_HANDLED;
			path_translated = apr_pstrdup(data->pools.process, path_translated);
			if (path_translated == NULL) return ELEMENT_NOT_HANDLED;
			dir = dirname(path_translated);
			full_path = templatizer_malloc(data, strlen(dir) + 1 + strlen(attr[i + 1]) + 1);
			if (full_path == NULL) {
				templatizer_free(data, path_translated);
				return ELEMENT_NOT_HANDLED;
			}
			sprintf(full_path, "%s/%s", dir, attr[i + 1]);
			templatizer_free(data, path_translated);
			if (parse_xml_file(data, full_path))
				return ELEMENT_NOT_HANDLED;
			templatizer_free(data, full_path);
		}
	}
	return ELEMENT_HANDLED;
}

static struct variable *add_variable
	(struct context *data,
	 struct variable_list_head *head)
{
	struct variable *n;
	n = (struct variable *) calloc(1, sizeof(struct variable));
	if (n == NULL)
		return NULL;
	TAILQ_INSERT_TAIL(head, n, entries);
	return n;
}

static int declare_variable_prototype(struct context *data, char *name, char *type)
{
	struct prototype *p = data->current_prototype;
	struct variable *var;
	if (p == NULL)
		return 1;
	if (strcmp(type, "string") == 0) {
		var = add_variable(data, &p->variables);
		var->name = name;
		var->type = type;
		p->num_words++;
	}
	return 0;
}

static struct prototype *create_prototype_node(struct context *data)
{
	struct prototype *n;
	(void) data;
	n = (struct prototype *) calloc(1, sizeof(struct prototype));
	if (n == NULL)
		return NULL;
	TAILQ_INIT(&n->variables);
	n->num_words = n->num_variables = 0;
	return n;
}

static int parse_prototype_tag(struct context *data, const XML_Char **attr)
{
	int i;
	struct prototype *n;

	n = create_prototype_node(data);

	for (i = 0; attr[i]; i += 2) {
		if (strcmp("name", attr[i]) == 0) {
			n->name = apr_pstrdup(data->pools.process, attr[i+1]);
		} else if (strcmp("return", attr[i]) == 0) {
			n->return_type = apr_pstrdup(data->pools.process, attr[i+1]);
		}
	}
	data->declare_variable = &declare_variable_prototype;
	return 0;
}

static struct import *create_import_node(struct context *data)
{
	void *p;
	(void) data;
	p = templatizer_malloc(data, sizeof(struct import));
	return (struct import *) p;
}

static int parse_extern_tag(struct context *data, const XML_Char **attr)
{
	int i;
	void *sym;
	struct import *n;
	char *name;

	for (i = 0; attr[i]; i += 2) {
		if (strcmp("name", attr[i]) == 0) {
			name = apr_pstrdup(data->pools.process, attr[i+1]);
			sym = get_symbol(data, name);
			n = create_import_node(data);
			n->name = name;
			n->address = sym;
			TAILQ_INSERT_TAIL(&data->imports, n, entries);
		}
	}
	return 0;
}

static int parse_string_tag(struct context *data, const XML_Char **attr)
{
	int i;
	char *name;

	for (i = 0; attr[i]; i += 2) {
		if (strcmp("name", attr[i]) == 0) {
			name = apr_pstrdup(data->pools.process, attr[i+1]);
		}
	}
	data->declare_variable(data, name, "string");
	return 0;
}

static char *tag_pool_add(struct context *data, const XML_Char *el)
{
	struct tag *new;

	if ((new = templatizer_malloc(data, sizeof(struct tag))) == NULL)
		return NULL;
	if ((new->s = apr_pstrdup(data->pools.process, el)) == NULL)
		return NULL;
	new->next = data->tag_head;
	data->tag_head = new;
	return new->s;
}

static char *tag_pool_lookup(struct context *data, const XML_Char *el)
{
	struct tag *p;

	for (p = data->tag_head; p; p = p->next) {
		if (strcmp(p->s, el) == 0)
			return p->s;
	}
	return tag_pool_add(data, el);
}

static struct node *add_node(struct context *data)
{
	struct node *n;

	if ((n = calloc(1, sizeof(struct node))) == NULL)
		goto nomem;
	TAILQ_INSERT_TAIL(data->nodes, n, entries);
	return n;
nomem:
	fprintf(stderr, "not enough memory for new node\n");
	XML_StopParser(data->parser, 0);
	return NULL;
}

static void free_attr_array(struct context *data, char **attr, int num_attributes)
{
	int i;

	for (i = 0; i < num_attributes; i++)
		templatizer_free(data, attr[i]);
	templatizer_free(data, attr);
}

enum variable_location {
	LOCATION_STACK,
	LOCATION_GLOBAL,
	LOCATION_CONSTANT
};

struct stack_entry {
	enum variable_location location;
	int value;
};

static int lookup_stack_variable
    (struct context *data,
     const char *name)
{
	return -1;
}

static int lookup_global_variable
    (struct context *data,
     const char *name)
{
	return -1;
}

static int create_param
    (struct context *data,
     struct stack_entry *entry,
     const XML_Char *attr)
{
	int index;
	if (isalpha(attr[0])) {
		if ((index = lookup_stack_variable(data, attr)) >= 0) {
			entry->location = LOCATION_STACK;
			entry->value = index;
		} else if ((index = lookup_global_variable(data, attr)) >= 0) {
			entry->location = LOCATION_GLOBAL;
			entry->value = index;
		} else {
			fprintf(stderr, "undefined variable: %s\n", attr);
			return 1;
		}
	} else if (isdigit(attr[0])) {
		entry->location = LOCATION_CONSTANT;
		entry->value = atoi(attr);
	}
	return 0;
}

static int add_callspecial_node(struct context *data, struct prototype *proto, const XML_Char **attr)
{
	struct variable *param;
	struct node *n;
	struct stack_entry *param_list;
	int num_params, position;
	int i;

	num_params = proto->num_variables;;
	if (num_params > 0) {
		param_list = (struct stack_entry *)
			calloc(num_params, sizeof(struct stack_entry));
		if (param_list == NULL)
			return 1;
	} else {
		param_list = NULL;
	}
	position = 0;
	TAILQ_FOREACH(param, &proto->variables, entries) {
		for (i = 0; attr[i]; i += 2) {
			if (strcmp(param->name, attr[i]) == 0)
				create_param(data, &param_list[position], attr[i+1]);
		}
		position++;
	}

	n = add_node(data);
	if (n == NULL) {
		templatizer_free(data, param_list);
		return 1;
	}
	return 0;
}

struct node *lookup_node_tag(tmpl_ctx_t ctx, const char *el)
{
	return NULL;
}

static int add_jump_node(tmpl_ctx_t data, const char *el)
{
	struct node *n = NULL;
	struct node *dest = NULL;
	n = add_node(data);
	if (n == NULL)
		return 1;
	dest = lookup_node_tag(data, el);
	if (dest == NULL)
		return 2;
	n->data.start.el = "jmp";
	n->data.start.num_attributes = 0;
	n->data.start.attr = NULL;
	n->data.start.jmp = dest;
	return 0;
}

static int parse_registered_start_tags(struct context *data, const char *el, const XML_Char **attr)
{
	struct prototype *p;
	struct element_start_callback *cb;
	int rc = -1;
	TAILQ_FOREACH(p, &data->prototypes, entries) {
		if (strcmp(p->name, el) == 0) {
			rc = add_jump_node(data, p->name);
			if (rc != 0)
				return ELEMENT_NOT_HANDLED;
			return ELEMENT_HANDLED;
		}
	}
	TAILQ_FOREACH(cb, data->on_element_start_callbacks, entries) {
		if (strcmp(cb->el, el) == 0) {
			rc = cb->f(data, el, attr);
			if (rc != 0) {
				data->error = 1;
				return ELEMENT_NOT_HANDLED;
			}
			return ELEMENT_HANDLED;
		}
	}
	return ELEMENT_NOT_HANDLED;
}

static int parse_registered_end_tags(struct context *data, const char *el)
{
	struct prototype *p;
	struct element_end_callback *cb;
	int rc = -1;
	TAILQ_FOREACH(p, &data->prototypes, entries) {
		if (strcmp(p->name, el) == 0) {
			/* Ignore end tags for script functions */
			return ELEMENT_HANDLED;
		}
	}
	TAILQ_FOREACH(cb, data->on_element_end_callbacks, entries) {
		if (strcmp(cb->el, el) == 0) {
			rc = cb->f(data, el);
			if (rc != 0) {
				data->error = 1;
				return ELEMENT_NOT_HANDLED;
			}
			return ELEMENT_HANDLED;
		}
	}
	return ELEMENT_NOT_HANDLED;
}

static int tmpl_add_start_node(tmpl_ctx_t data, const char *el, const char **attr)
{
	int i;
	void *p;
	struct node *n;
	/* unregistered and unknown tags */
	n = add_node(data);
	if (n == NULL) return 1;
	n->type = NODE_START;
	if ((n->data.start.el = tag_pool_lookup(data, el)) == NULL) {
		fputs("no memory for element tag\n", stderr);
		XML_StopParser(data->parser, 0);
		data->error = 1;
		return ELEMENT_NOT_HANDLED;
	}
	for (i = 0; attr[i]; i++);
	if (i > 0) {
		if ((p = templatizer_malloc(data, i * sizeof(char *))) == NULL) {
			templatizer_free(data, n->data.start.el);
			fputs("no memory for new attribute\n", stderr);
			XML_StopParser(data->parser, 0);
			data->error = 1;
			return ELEMENT_NOT_HANDLED;
		}
	} else {
		p = NULL;
	}
	n->data.start.num_attributes = i;
	n->data.start.attr = (char **) p;
	n->data.start.jmp = NULL;
	for (i = 0; attr[i]; i++) {
		if ((n->data.start.attr[i] = apr_pstrdup(data->pools.process, attr[i])) == NULL) {
			templatizer_free(data, n->data.start.el);
			free_attr_array(data, n->data.start.attr, i);
			data->error = 1;
			return ELEMENT_NOT_HANDLED;
		}
	}
	if (is_control_flow_keyword(el)) {
		if (data->num_labels >= MAX_LABELS) {
			templatizer_free(data, n->data.start.el);
			free_attr_array(data, n->data.start.attr, n->data.start.num_attributes);
			fputs("control flow stack overflow\n", stderr);
			XML_StopParser(data->parser, 0);
			data->error = 1;
			return ELEMENT_NOT_HANDLED;
		}
		data->labels[data->num_labels++] = n;
	}
	data->error = 0;
	return ELEMENT_HANDLED;
}

static int parse_unregistered_start_tag(struct context *data, const XML_Char *el, const XML_Char **attr)
{
	int rc = ELEMENT_NOT_HANDLED;
	rc = tmpl_add_start_node(data, el, attr);
	if (rc == ELEMENT_HANDLED)
		return 0;
	return 1;
}

static int parse_keyword_tags(struct context *data, const XML_Char *el, const XML_Char **attr)
{
	int rc = ELEMENT_NOT_HANDLED;
	if (strcmp("templatizer", el) == 0) {
		rc = parse_templatizer_tag(data, attr);
	} else if (strcmp("include", el) == 0) {
		rc = parse_include_tag(data, attr);
	} else if (strcmp("extern", el) == 0) {
		parse_extern_tag(data, attr);
		rc = ELEMENT_HANDLED;
	} else if (strcmp("prototype", el) == 0) {
		parse_prototype_tag(data, attr);
		rc = ELEMENT_HANDLED;
	} else if (strcmp("string", el) == 0) {
		parse_string_tag(data, attr);
		rc = ELEMENT_HANDLED;
	}
	return rc;
}

static bool parse_element_start(struct context *data, const XML_Char *el, const XML_Char **attr)
{
    bool rc = -1;

    rc = parse_keyword_tags(data, el, attr);
    if (rc == ELEMENT_HANDLED) {
        return true;
    } else {
        if (data->error != 0)
            return false;
    }
    rc = parse_registered_start_tags(data, el, attr);
    if (rc == ELEMENT_HANDLED) {
        return true;
    } else {
        if (data->error != 0)
            return false;
    }
    rc = parse_unregistered_start_tag(data, el, attr);
    if (rc == ELEMENT_HANDLED) {
        return true;
    } else {
        if (data->error != 0)
            return false;
    }
    return true;
}

static struct node *add_end_node(tmpl_ctx_t data, const char *el, bool selfclosing)
{
	struct node *n;
	n = add_node(data);
	if (n == NULL) return NULL;
	n->type = NODE_END;
	n->data.end.el = tag_pool_lookup(data, el);
	n->data.end.selfclosing = selfclosing;
	return n;
}

static int tmpl_add_end_node(tmpl_ctx_t ctx, const char *el)
{
	struct node *n = NULL;
	n = add_end_node(ctx, el, false);
	if (n == NULL)
		return 1;
	n->data.end.jmp = NULL;
	return 0;
}

static bool tmpl_add_if_end_node(tmpl_ctx_t data)
{
	const char *eif = "if";
	struct node *n = add_end_node(data, eif, false);
	struct node *if_start_node = NULL;
	/* The previous label (start tag) will jump to this node. */
	if_start_node = data->labels[--data->num_labels];
	if (if_start_node == NULL)
		return false;
	if (strcmp(if_start_node->data.start.el, eif) != 0)
		return false;
	if_start_node->data.start.jmp = n;
	n->data.end.jmp = NULL; /* This node does not jump. */
	n->data.end.conditional_jmp = false;
	return true;
}

static bool tmpl_add_swhile_end_node(tmpl_ctx_t data)
{
	const char *swhile = "swhile";
	struct node *n = add_end_node(data, swhile, false);
	struct node *swhile_start_node = NULL;
	data->num_labels--;
	swhile_start_node = data->labels[data->num_labels];
	if (swhile_start_node == NULL)
		return false;
	if (strcmp(swhile_start_node->data.start.el, swhile) != 0)
		return false;
	/* The previous label (start tag) will jump to this node. */
	swhile_start_node->data.start.jmp = n;
	/* This node will jump to the previous label (start tag). */
	n->data.end.jmp = data->labels[data->num_labels];
	n->data.end.conditional_jmp = false;
	return true;
}

static bool tmpl_add_ewhile_end_node(tmpl_ctx_t ctx)
{
	const char *ewhile = "ewhile";
	struct node *n = add_end_node(ctx, ewhile, false);
	struct node *ewhile_start_node = NULL;
	ewhile_start_node = ctx->labels[--ctx->num_labels];
	if (ewhile_start_node == NULL)
		return false;
	if (strcmp(ewhile_start_node->data.end.el, ewhile) != 0)
		return false;
	ewhile_start_node->data.start.jmp = NULL;
	/* This node will jump to the previous label (start tag). */
	n->data.end.jmp = ewhile_start_node;
	n->data.end.conditional_jmp = true;
	return true;
}

static int parse_unregistred_end_tag(tmpl_ctx_t ctx, const char *el)
{
	bool rc = false;
	if (strcmp("ewhile", el) == 0) {
		rc = tmpl_add_ewhile_end_node(ctx);
	} else if (strcmp("if", el) == 0) {
		rc = tmpl_add_if_end_node(ctx);
	} else if (strcmp("swhile", el) == 0) {
		rc = tmpl_add_swhile_end_node(ctx);
	} else {
		rc = tmpl_add_end_node(ctx, el);
	}
	return rc;
}

int parse_element_end(tmpl_ctx_t ctx, const char *el)
{
	int rc = -1;

	if (strcmp("include", el) == 0)
		return true;
	if (strcmp("extern", el) == 0)
		return true;
	if (strcmp("prototype", el) == 0)
		return true;
	if (strcmp("string", el) == 0)
		return true;
	rc = parse_registered_end_tags(ctx, el);
	if (rc == ELEMENT_HANDLED) {
		return true;
	} else {
		if (ctx->error != 0)
			return false;
	}
	rc = parse_unregistred_end_tag(ctx, el);
	if (rc != 0)
		return false;
	return true;
}

static int tmpl_add_selfclosing_html_node(tmpl_ctx_t ctx, const char *el, tmpl_attr_t attr)
{
	bool rc = false;
	rc = parse_element_start(ctx, el, attr);
	if (rc == false)
		return 1;
	rc = parse_element_end(ctx, el);
	if (rc == false)
		return 2;
	return 0;
}

static void start(struct context *data, const XML_Char *el, const XML_Char **attr)
{
	if (parse_element_start(data, el, attr)) {
		XML_StopParser(data->parser, 0);
	}
}

static void end(struct context *data, const XML_Char *el)
{
	if (parse_element_end(data, el)) {
		XML_StopParser(data->parser, 0);
	}
}

static void character_data(struct context *data, const XML_Char *s, int len)
{
	struct node *n;

	n = add_node(data);
	if (n == NULL) return;
	n->type = NODE_CHARACTER_DATA;
#if 0
	while (len > 0 && isspace(s[len-1]))
		len--;
	while (len > 0 && isspace(*s)) {
		s++;
		len--;
	}
#endif
	n->data.character_data.data = strndup(s, len);
}

static void free_tag_pool(struct context *data)
{
	struct tag *p, *next;

	for (p = data->tag_head; p; p = next) {
		next = p->next;
		templatizer_free(data, p->s);
		templatizer_free(data, p);
	}
}

static void free_all_nodes(struct context *data)
{
	struct node *p;
	struct node *last_node = NULL;

	TAILQ_FOREACH(p, data->nodes, entries) {
		if (p->type == NODE_START) {
			free_attr_array(data, p->data.start.attr, p->data.start.num_attributes);
		} else if (p->type == NODE_END) {
		} else if (p->type == NODE_CHARACTER_DATA) {
			templatizer_free(data, p->data.character_data.data);
		}
		templatizer_free(data, last_node);
		last_node = p;
	};
	templatizer_free(data, last_node);
	templatizer_free(data, data->nodes);
}

static void free_all_input(struct context *data)
{
	struct input *p;
	struct input *last_node = NULL;

	TAILQ_FOREACH(p, data->input, entries) {
		if (p->type == INPUT_FILLER_TEXT) {
			templatizer_free(data, p->data.filler_text);
		} else if (p->type == INPUT_CONTROL_FLOW) {
		}
		templatizer_free(data, last_node);
		last_node = p;
	};
	templatizer_free(data, last_node);
	templatizer_free(data, data->input);
}

static int parse_xml_file(struct context *data, const char *tmpl)
{
	XML_Parser parser;
	FILE *file;
	int r = 1;
	int done = 0;
	int num_items;
	char buffer[BUFFER_SIZE];

	parser = XML_ParserCreate(NULL);
	XML_SetUserData(parser, data);
	XML_SetElementHandler(parser,
		(XML_StartElementHandler) start,
		(XML_EndElementHandler) end);
	XML_SetCharacterDataHandler(parser, (XML_CharacterDataHandler) character_data);
	//file = fopen(tmpl, "rt");
	file = open_path_translated(data, tmpl);
	if (file == NULL) {
		fprintf(stderr, "%s: unable to open file '%s'.\n", __FUNCTION__, tmpl);
		fputs("Status: 404 File not found\r\n", stdout);
		fputs("Content-Type: text/plain\r\n", stdout);
		fputs("\r\n", stdout);
		puts("File not found.");
		return 1;
	}
	do {
		num_items = fread(buffer, 1, BUFFER_SIZE, file);
		if (ferror(file))
			goto out;
		if (feof(file))
			done = 1;
		if (!XML_Parse(parser, buffer, num_items, done)) {
			fprintf(stderr, "ERROR: parse error at line %d: %s\n",
				(int) XML_GetCurrentLineNumber(parser),
				XML_ErrorString(XML_GetErrorCode(parser)));
			fflush(stderr);
			goto out;
		}
	} while(!done);
	XML_ParserFree(parser);
	r = 0;
out:
	fclose(file);
	return r;
}

static FILE *open_path_translated(tmpl_ctx_t data, const char *pathtranslated)
{
	char *token, *string, *tofree;
	struct stat statbuf;
	int fd = 0;
	FILE *file = NULL;
#ifdef _WIN32
	const char separators[] = "/\\";
#else
	const char separators[] = "/";
#endif
	const char *documentroot = getenv("DOCUMENT_ROOT");
	tofree = string = templatizer_strdup(data, pathtranslated);
	if (string == NULL)
		return NULL;
	if (strcspn(string, separators) == 0) {
#ifdef TERMUX
		fd = open(TERMUX_ROOT_DIR, O_RDONLY);
		string += strlen(TERMUX_ROOT_DIR);
#else
		fd = open("/", O_RDONLY);
		string++;
#endif
	} else if (documentroot != NULL) {
		fd = open(documentroot, O_RDONLY);
	} else {
		fd = open(".", O_RDONLY);
	}
	if (fd < 0) {
		fputs("Unable to open root directory as read-only.\n", stderr);
		goto out1;
	}
	while ((token = strsep(&string, separators)) != NULL) {
		if (token[0] == '.') {
			close(fd);
			goto out1;
		}
		fd = openat(fd, token, O_RDONLY);
		if (fd < 0) {
			fprintf(stderr, "Unable to open node '%s'\n", token);
			goto out1;
		}
		memset(&statbuf, 0, sizeof(statbuf));
		fstat(fd, &statbuf);
		if ((statbuf.st_mode & S_IFMT) == S_IFREG)
			break;
	}
	if (string) {
		data->script_path = strndup(pathtranslated, string - tofree);
		data->path_info = templatizer_strdup(data, string);
	} else {
		data->script_path = pathtranslated;
		data->path_info = "";
	}
	file = fdopen(fd, "rt");
	if (file == NULL) {
		fprintf(stderr, "fdopen failed for file '%s' in '%s'\n", token, pathtranslated);
		goto out1;
	}
out1:
	templatizer_free(data, tofree);
	return file;
}

void parse_command_line()
{
	/*static struct option long_options[] = {
		{"daemon",  required_argument, 0, 'd' },
		{"verbose", no_argument,       0, 'v' },
		{"compile", required_argument, 0, 'c'},
		{"file",    required_argument, 0, 'f' },
		{0,         0,                 0,  0 }
        };*/
}

int main(int argc, char **argv)
{
	char *tmpl;
	struct context data;
	apr_status_t status;
	clock_t begin;
	clock_t end;
	double time_spent;
	FILE *log;
	const char *gateway_interface;
	int rc = -1;

	(void) argc;

	begin = clock();

	log = fopen("/var/log/templatizer/access.log", "a");
	if (log == NULL) {
		log = fopen("access.log", "a");
		if (log == NULL) {
			fputs("Unable to open log file.\n", stderr);
			return 1;
		}
	}

	memset(&data, 0, sizeof(data));

	gateway_interface = getenv("GATEWAY_INTERFACE");
	tmpl = getenv("PATH_TRANSLATED");

	if (gateway_interface == NULL) {
		/* parse arguments with getopt */
		parse_command_line();
	}

	if (tmpl == NULL) {
		fprintf(stderr, "%s: missing template file\n", argv[0]);
		return 1;
	}

	apr_initialize();
	status = apr_pool_create(&data.pools.process, NULL);
	status = apr_pool_create(&data.pools.connection, NULL);
	if (status != 0) {
		fputs("Failed to create process pool.\n", stderr);
		return 1;
	}

#ifdef USE_STORAGE
	rc = storage_initialize();
	assert(rc == 0);
#endif

	/*data.parser = parser;*/
	if ((data.nodes = calloc(1, sizeof(struct node_list_head))) == NULL)
		return 1;
	TAILQ_INIT(data.nodes);
	if ((data.input = calloc(1, sizeof(struct input))) == NULL)
		return 1;
	data.on_element_start_callbacks = tmpl_process_malloc(&data, sizeof(struct element_start_callback_head));
	if (data.on_element_start_callbacks == NULL)
		return 1;
	data.on_element_end_callbacks = tmpl_process_malloc(&data, sizeof(struct element_end_callback_head));
	if (data.on_element_end_callbacks == NULL)
		return 1;
	data.plugin_variables =
		tmpl_process_malloc(&data, sizeof(*data.plugin_variables));
	data.num_labels = 0;
	data.declare_variable = NULL;
	TAILQ_INIT(data.input);
	TAILQ_INIT(&data.imports);
	TAILQ_INIT(&data.prototypes);
	TAILQ_INIT(data.on_element_start_callbacks);
	TAILQ_INIT(data.on_element_end_callbacks);
	data.tag_head = NULL;
	data.plugin_handle = NULL;
	data.output_format = TMPL_FMT_XHTML;
	data.output_compression = TMPL_Z_PLAIN; /* NOTE: remember CRIME and BREACH exploits. */
	data.keep_alive = false;
	parse_xml_file(&data, tmpl);
	serialize_template_file(&data);
	print_list(&data); /* Interpreter */
	apr_pool_destroy(data.pools.connection);

#if 1
	if (0) {
		free_tag_pool(&data);
		free_all_nodes(&data);
		free_all_input(&data); /* free unused input */
	} else {
		apr_pool_destroy(data.pools.process);
	}
	if (data.plugin_handle)
		unload_library(&data);
#endif

#ifdef USE_STORAGE
	rc = storage_finalize();
	assert(rc == 0);
#endif

	apr_terminate();

	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	fprintf(log, "[%lf] %s\n", time_spent, tmpl);
	fclose(log);

	return 0;
}

