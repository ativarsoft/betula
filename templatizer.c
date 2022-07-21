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

#define BUFFER_SIZE 4096
#define MAX_LABELS 128

#define VERSION "0.1"
#define COPYRIGHT "Copyright (C) 2017-2022 Mateus de Lima Oliveira"

static int parse_xml_file(struct context *data, const char *tmpl);

const char version[] = VERSION;
const char copyright[] = COPYRIGHT;

enum node_type {
	NODE_START,
	NODE_END,
	NODE_CHARACTER_DATA
};

struct node_start {
	char *el;
	int num_attributes;
	char **attr;
	struct node *jmp; /* node_end */
};

struct node_end {
	char *el;
	bool conditional_jmp;
	struct node *jmp; /* node_start */
};

struct node_character_data {
	char *data;
};

union node_data {
	struct node_start start;
	struct node_end end;
	struct node_character_data character_data;
};

struct node {
	enum node_type type;
	TAILQ_ENTRY(node) entries;
	union node_data data;
};

TAILQ_HEAD(node_list_head, node);

struct import {
	TAILQ_ENTRY(import) entries;
	char *name;
	void *address;
};

TAILQ_HEAD(import_list_head, import);

enum input_type {
	INPUT_FILLER_TEXT,
	INPUT_CONTROL_FLOW
};

struct input {
	TAILQ_ENTRY(input) entries;
	enum input_type type;
	union {
		char *filler_text;
		bool control_flow;
	} data;
};

TAILQ_HEAD(input_list_head, input);

struct element_callback {
	TAILQ_ENTRY(element_callback) entries;
};

TAILQ_HEAD(element_callback_head, element_callback);

struct tag {
	struct tag *next;
	XML_Char *s;
};

struct context {
	struct node_list_head *nodes;
	struct import_list_head imports;
	/* This list is freed as it is consumed. */
	struct input_list_head *input;
	struct tag *tag_head;
	struct node *labels[MAX_LABELS];
	struct program *bin;
	struct element_callback_head *on_element_callbacks;
	int num_labels;
	int status; /* value returned by the last call */
	XML_Parser parser;

	void *plugin_handle;
	struct templatizer_plugin *plugin_data;
	enum templatizer_format output_format;
	enum templatizer_compression output_compression;
	bool keep_alive;
};

static struct context *get_tmpl_context(tmpl_ctx_t ctx)
{
	return (struct context *) ctx;
}

static void *templatizer_malloc(struct context *data, size_t size)
{
	(void) data;
	return malloc(size);
}

static void templatizer_free(struct context *data, void *ptr)
{
	(void) data;
	free(ptr);
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

	if ((n = malloc(sizeof(struct input))) == NULL)
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
	if ((p->data.filler_text = strdup(text)) == NULL)
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
	&templatizer_malloc,
	&templatizer_free,
	&set_compression,
	&set_keep_alive,
	&send_header,
	&send_default_headers,
	&set_output_format,
	&add_filler_text,
	&add_control_flow,
	&register_element_tag,
	&tmpl_exit
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
	path_translated = strdup(path_translated);
	if (path_translated == NULL) return NULL;
	dir = dirname(path_translated);
	full_path = malloc(strlen(dir) + 1 + strlen(path) + 1);
	if (full_path == NULL) {
		free(path_translated);
		return NULL;
	}
	sprintf(full_path, "%s/%s", dir, path);
	free(path_translated);
	handle = dlopen(full_path, RTLD_LAZY);
	if (handle == NULL) {
		fprintf(stderr, "%s\n", dlerror());
		free(full_path);
		return NULL;
	}
	data->plugin_data = dlsym(handle, "templatizer_plugin_v1");
	if (data->plugin_data == NULL) {
		dlclose(handle);
		fprintf(stderr, "%s\n", dlerror());
		free(full_path);
		return NULL;
	}
	free(full_path);
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
			if (path_translated == NULL) return 1;
			path_translated = strdup(path_translated);
			if (path_translated == NULL) return 1;
			dir = dirname(path_translated);
			full_path = malloc(strlen(dir) + 1 + strlen(attr[i + 1]) + 1);
			if (full_path == NULL) {
				free(path_translated);
				return 1;
			}
			sprintf(full_path, "%s/%s", dir, attr[i + 1]);
			free(path_translated);
			if (parse_xml_file(data, full_path))
				return 1;
			free(full_path);
		}
	}
	return 0;
}

static struct import *create_import_node(struct context *data)
{
	void *p;
	(void) data;
	p = malloc(sizeof(struct import));
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
			name = strdup(attr[i+1]);
			sym = get_symbol(data, name);
			n = create_import_node(data);
			n->name = name;
			n->address = sym;
			TAILQ_INSERT_TAIL(&data->imports, n, entries);
		}
	}
	return 0;
}

static char *tag_pool_add(struct context *data, const XML_Char *el)
{
	struct tag *new;

	if ((new = malloc(sizeof(struct tag))) == NULL)
		return NULL;
	if ((new->s = strdup(el)) == NULL)
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

static void dump_string(struct context *data)
{
	struct input *p;

	if (TAILQ_EMPTY(data->input))
		return;
	p = TAILQ_FIRST(data->input);
	fputs(p->data.filler_text, stdout);
	TAILQ_REMOVE(data->input, p, entries);
	free(p);
}

/* NOTE: not all keywords here are related to control flow anymore. */
static bool is_control_flow_keyword(const XML_Char *el)
{
	if (strcmp("if", el) == 0
		|| strcmp("swhile", el) == 0
		|| strcmp("ewhile", el) == 0)
		return true;
	return false;
}

static void print_start_node(struct context *data, struct node_start *n)
{
	int i;

	if (strcmp("templatizer", n->el) == 0)
		return;
	if (is_control_flow_keyword(n->el))
		return;
	putchar('<');
	fputs(n->el, stdout);
	if (n->attr) {
		for (i = 0; i < n->num_attributes; i += 2) {
			if (strcmp("@", n->attr[i + 1]) == 0) {
				printf(" %s=\"", n->attr[i]);
				dump_string(data);
				putchar('"');
			} else {
				printf(" %s=\"%s\"", n->attr[i], n->attr[i + 1]);
			}
		}
	}
	putchar('>');
}

static void print_end_node(struct node_end *n)
{
	if (strcmp("templatizer", n->el) == 0)
		return;
	if (is_control_flow_keyword(n->el))
		return;
	printf("</%s>", n->el);
}

static void print_character_data_node(struct context *data, struct node_character_data *n)
{
	char *s;
	char c;

	for (s = n->data; (c = *s) != '\0'; s++) {
		if (c == '@')
			dump_string(data);
		else
			putchar(c);
	}
}

enum control_flow {
	JMP_BACKWARD,
	JMP_FOWARD,
	NEXT_INSTRUCTION
};

static enum control_flow print_node(struct context *data, struct node *n)
{
	struct input *p;
	enum control_flow r;

	switch (n->type) {
	case NODE_START:
		if (n->data.start.jmp) {
			if (TAILQ_EMPTY(data->input)) {
				fprintf(stderr, "missing input for tag that requires input\n");
				exit(1);
			}
			p = TAILQ_FIRST(data->input);
			if (p->data.control_flow)
				r = JMP_FOWARD;
			else
				r = NEXT_INSTRUCTION;
			TAILQ_REMOVE(data->input, p, entries);
			free(p);
			return r;
		}
		print_start_node(data, &n->data.start);
		break;
	case NODE_END:
		if (n->data.end.jmp) {
			if (n->data.end.conditional_jmp) {
				if (0) /* TODO */
					return JMP_BACKWARD;
				else
					return NEXT_INSTRUCTION;
			} else {
				return JMP_BACKWARD;
			}
		}
		print_end_node(&n->data.end);
		break;
	case NODE_CHARACTER_DATA:
		print_character_data_node(data, &n->data.character_data);
		break;
	}
	return NEXT_INSTRUCTION;
}

static void print_list(struct context *data)
{
	struct node *p;

	p = TAILQ_FIRST(data->nodes);
	while (p != TAILQ_LAST(data->nodes, node_list_head)) {
		switch (print_node(data, p)) {
			case JMP_FOWARD:
			p = TAILQ_NEXT(p->data.start.jmp, entries);
			break;
			case JMP_BACKWARD:
			p = p->data.end.jmp;
			break;
			default:
			p = TAILQ_NEXT(p, entries);
			break;
		}
#if 1
		fflush(stdout);
#endif
	}
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

static void free_attr_array(char **attr, int num_attributes)
{
	int i;

	for (i = 0; i < num_attributes; i++)
		free(attr[i]);
	free(attr);
}

static void start(struct context *data, const XML_Char *el, const XML_Char **attr)
{
	int i;
	void *p;
	struct node *n;

	if (strcmp("templatizer", el) == 0) {
		parse_templatizer_tag(data, attr);
		return;
	}
	if (strcmp("include", el) == 0) {
		parse_include_tag(data, attr);
		return;
	}
	if (strcmp("extern", el) == 0) {
		parse_extern_tag(data, attr);
		return;
	}
	n = add_node(data);
	if (n == NULL) return;
	n->type = NODE_START;
	if ((n->data.start.el = tag_pool_lookup(data, el)) == NULL) {
		fputs("no memory for element tag\n", stderr);
		XML_StopParser(data->parser, 0);
		return;
	}
	for (i = 0; attr[i]; i++);
	if (i > 0) {
		if ((p = malloc(i * sizeof(char *))) == NULL) {
			free(n->data.start.el);
			fputs("no memory for new attribute\n", stderr);
			XML_StopParser(data->parser, 0);
			return;
		}
	} else {
		p = NULL;
	}
	n->data.start.num_attributes = i;
	n->data.start.attr = (char **) p;
	n->data.start.jmp = NULL;
	for (i = 0; attr[i]; i++) {
		if ((n->data.start.attr[i] = strdup(attr[i])) == NULL) {
			free(n->data.start.el);
			free_attr_array(n->data.start.attr, i);
			return;
		}
	}
	if (is_control_flow_keyword(el)) {
		if (data->num_labels >= MAX_LABELS) {
			free(n->data.start.el);
			free_attr_array(n->data.start.attr, n->data.start.num_attributes);
			fputs("control flow stack overflow\n", stderr);
			XML_StopParser(data->parser, 0);
			return;
		}
		data->labels[data->num_labels++] = n;
	}
}

static void end(struct context *data, const XML_Char *el)
{
	struct node *n;

	if (strcmp("include", el) == 0)
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
		free(p->s);
		free(p);
	}
}

static void free_all_nodes(struct context *data)
{
	struct node *p;

	TAILQ_FOREACH(p, data->nodes, entries) {
		if (p->type == NODE_START) {
			free_attr_array(p->data.start.attr, p->data.start.num_attributes);
		} else if (p->type == NODE_END) {
		} else if (p->type == NODE_CHARACTER_DATA) {
			free(p->data.character_data.data);
		}
		free(p);
	};
	free(data->nodes);
}

static void free_all_input(struct context *data)
{
	struct input *p;

	TAILQ_FOREACH(p, data->input, entries) {
		if (p->type == INPUT_FILLER_TEXT) {
			free(p->data.filler_text);
		} else if (p->type == INPUT_CONTROL_FLOW) {
		}
		free(p);
	};
	free(data->input);
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
	file = fopen(tmpl, "rt");
	if (file == NULL) {
		fprintf(stderr, "%s: unable to open file '%s'.\n", __FUNCTION__, tmpl);
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

int main(int argc, char **argv)
{
	char *tmpl;
	struct context data;

	(void) argc;

	memset(&data, 0, sizeof(data));

	tmpl = getenv("PATH_TRANSLATED");
	if (tmpl == NULL) {
		fprintf(stderr, "%s: missing template file\n", argv[0]);
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
	TAILQ_INIT(data.input);
	TAILQ_INIT(&data.imports);
	data.tag_head = NULL;
	data.plugin_handle = NULL;
	data.output_format = TMPL_FMT_XHTML;
	data.output_compression = TMPL_Z_PLAIN; /* NOTE: remember CRIME and BREACH exploits. */
	data.keep_alive = false;
	parse_xml_file(&data, tmpl);
	print_list(&data);
#if 1
	free_tag_pool(&data);
	free_all_nodes(&data);
	free_all_input(&data); /* free unused input */
	if (data.plugin_handle)
		unload_library(&data);
#endif
	return 0;
}

