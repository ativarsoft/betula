/* Copyright (C) 2017-2022 Mateus de Lima Oliveira */

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
#include <apr-1.0/apr_pools.h>
#include <apr-1.0/apr_strings.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <templatizer/compiler/compiler.h>
#include "memory.h"
#include "interpreter.h"
#include "opcode.h"

#define VERSION "0.1"
#define COPYRIGHT "Copyright (C) 2017-2022 Mateus de Lima Oliveira"

static int parse_xml_file(struct context *data, const char *tmpl);

const char version[] = VERSION;
const char copyright[] = COPYRIGHT;

enum {
    ELEMENT_HANDLED = 0,
    ELEMENT_NOT_HANDLED = 1
};

/* Function prototypes */
static struct node *add_node(struct context *data);
static FILE *open_path_translated(tmpl_ctx_t data, const char *pathtranslated);

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
		content_type = "text/html";
		break;
		case TMPL_FMT_XHTML:
		content_type = "application/xhtml+xml";
		break;
	}
	send_header(data, "Content-Type", content_type);
	send_header(data, "X-Powered-By", "Templatizer " VERSION);
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

static int register_element_tag(tmpl_ctx_t ctx, char *s, on_element_callback_t cb)
{
	struct context *data = get_tmpl_context(ctx);
	struct element_callback *p;
	p = calloc(1, sizeof(struct element_callback));
	if (p == NULL)
		return 1;
	TAILQ_INSERT_TAIL(data->on_element_callbacks, p, entries);
	return 0;
}

static void tmpl_exit(tmpl_ctx_t ctx, int status)
{
	struct context *data = get_tmpl_context(ctx);
	data->status = status;
}

static struct templatizer_callbacks callbacks = {
	.malloc = &templatizer_malloc,
	.free = &templatizer_free,
	.set_compression = &set_compression,
	.set_keep_alive = &set_keep_alive,
	.send_header = &send_header,
	.send_default_headers = &send_default_headers,
	.set_output_format = &set_output_format,
	.add_filler_text = &add_filler_text,
	.add_control_flow = &add_control_flow,
	.register_element_tag = &register_element_tag,
	.exit = &tmpl_exit
};

#ifdef _WIN32
#error win32 is not supported yet
#else
static void *load_library(struct context *data, const char *path)
{
	void *handle;
	char *path_translated, *dir, *full_path;

	path_translated = getenv("PATH_TRANSLATED");
	if (path_translated == NULL) return NULL;
	path_translated = apr_pstrdup(data->pools.process, path_translated);
	if (path_translated == NULL) return NULL;
	dir = dirname(path_translated);
	full_path = templatizer_malloc(data, strlen(dir) + 1 + strlen(path) + 1);
	if (full_path == NULL) {
		templatizer_free(data, path_translated);
		return NULL;
	}
	sprintf(full_path, "%s/%s", dir, path);	templatizer_free(data, path_translated);
	handle = dlopen(full_path, RTLD_LAZY);
	if (handle == NULL) {
		fprintf(stderr, "%s\n", dlerror());
		templatizer_free(data, full_path);
		return NULL;
	}
	data->plugin_data = dlsym(handle, "templatizer_plugin_v1");
	if (data->plugin_data == NULL) {
		dlclose(handle);
		fprintf(stderr, "%s\n", dlerror());
		templatizer_free(data, full_path);
		return NULL;
	}
	templatizer_free(data, full_path);
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
				fprintf(stderr, "unable to load library '%s'", attr[i + 1]);
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

static int parse_registered_tags(struct context *data, const XML_Char *el, const XML_Char **attr)
{
	struct prototype *p;
	TAILQ_FOREACH(p, &data->prototypes, entries) {
		if (strcmp(p->name, el) == 0)
			return add_callspecial_node(data, p, attr);
	}
	return ELEMENT_HANDLED;
}

static int parse_unregistered_node(struct context *data, const XML_Char *el, const XML_Char **attr)
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
    /*rc = parse_registered_tags(data, el, attr);
    if (rc == ELEMENT_HANDLED) {
        return true;
    } else {
        if (data->error != 0)
            return false;
    }*/
    rc = parse_unregistered_node(data, el, attr);
    if (rc == ELEMENT_HANDLED) {
        return true;
    } else {
        if (data->error != 0)
            return false;
    }
    return true;
}

static void start(struct context *data, const XML_Char *el, const XML_Char **attr)
{
	if (parse_element_start(data, el, attr)) {
		XML_StopParser(data->parser, 0);
	}
}

static void end(struct context *data, const XML_Char *el)
{
	struct node *n;

	if (strcmp("include", el) == 0)
		return;
	if (strcmp("extern", el) == 0)
		return;
	if (strcmp("prototype", el) == 0)
		return;
	if (strcmp("string", el) == 0)
		return;
	n = add_node(data);
	if (n == NULL) return;
	n->type = NODE_END;
	n->data.end.el = tag_pool_lookup(data, el);
	if (strcmp("ewhile", el) == 0) {
		/* This node will jump to the previous label (start tag). */
		n->data.end.jmp = data->labels[--data->num_labels];
		n->data.end.conditional_jmp = true;
	} else if (strcmp("if", el) == 0) {
		/* The previous label (start tag) will jump to this node. */
		data->labels[--data->num_labels]->data.start.jmp = n;
		n->data.end.jmp = NULL; /* This node does not jump. */
	} else if (strcmp("swhile", el) == 0) {
		data->num_labels--;
		/* The previous label (start tag) will jump to this node. */
		data->labels[data->num_labels]->data.start.jmp = n;
		/* This node will jump to the previous label (start tag). */
		n->data.end.jmp = data->labels[data->num_labels];
		n->data.end.conditional_jmp = false;
	} else {
		n->data.end.jmp = NULL;
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
		fd = open("/", O_RDONLY);
		string++;
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
	if (status != 0) {
		fputs("Failed to create process pool.\n", stderr);
		return 1;
	}

	/*data.parser = parser;*/
	if ((data.nodes = calloc(1, sizeof(struct node_list_head))) == NULL)
		return 1;
	TAILQ_INIT(data.nodes);
	if ((data.input = calloc(1, sizeof(struct input))) == NULL)
		return 1;
	data.on_element_callbacks = calloc(1, sizeof(struct element_callback_head));
	if (data.on_element_callbacks == NULL)
		return 1;
	data.num_labels = 0;
	data.declare_variable = NULL;
	TAILQ_INIT(data.input);
	TAILQ_INIT(&data.imports);
	TAILQ_INIT(&data.prototypes);
	data.tag_head = NULL;
	data.plugin_handle = NULL;
	data.output_format = TMPL_FMT_XHTML;
	data.output_compression = TMPL_Z_PLAIN; /* NOTE: remember CRIME and BREACH exploits. */
	data.keep_alive = false;
	parse_xml_file(&data, tmpl);
	serialize_template_file(&data);
	print_list(&data);

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
	apr_terminate();

	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	fprintf(log, "[%lf] %s\n", time_spent, tmpl);
	fclose(log);

	return 0;
}

