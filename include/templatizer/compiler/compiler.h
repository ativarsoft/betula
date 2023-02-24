/* Copyright (C) 2017-2022 Mateus de Lima Oliveira */

#ifndef TMPL_COMPILER_H
#define TMPL_COMPILER_H

#include <sys/queue.h>
#include <expat.h>
#include <apr_pools.h>
#include <apr_strings.h>
#include <stdbool.h>

#define BUFFER_SIZE 4096
#define MAX_LABELS 128

typedef int (*declare_variable_t)(struct context *data, char *name, char *type);
typedef apr_pool_t *tmpl_pool_t;

struct plugin_variable;

struct variable {
	TAILQ_ENTRY(variable) entries;
	char *name;
	char *type;
};

TAILQ_HEAD(variable_list_head, variable);

struct prototype {
	TAILQ_ENTRY(prototype) entries;
	char *return_type;
	char *name;
	int num_words; /* number of stack frame words */
	int num_variables;
	struct variable_list_head variables;
};

TAILQ_HEAD(prototype_list_head, prototype);

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
	int selfclosing;
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

typedef struct input *input_t;

struct element_start_callback {
	TAILQ_ENTRY(element_callback) entries;
	const char *el;
	on_element_start_callback_t f;
};

struct element_end_callback {
	TAILQ_ENTRY(element_callback) entries;
	const char *el;
	on_element_end_callback_t f;
};

TAILQ_HEAD(element_start_callback_head, element_start_callback);
TAILQ_HEAD(element_end_callback_head, element_end_callback);

struct plugin_parameters {
	TAILQ_ENTRY(plugin_parameters) entries;
	const char *param_ptr;
	size_t param_length; /* max space allocated for param */
};

TAILQ_HEAD(plugin_parameters_head, plugin_parameters);

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
	struct element_start_callback_head *on_element_start_callbacks;
	struct element_end_callback_head *on_element_end_callbacks;
	struct prototype_list_head prototypes;
	int num_labels;
	int status; /* value returned by the last call */
	XML_Parser parser;
	declare_variable_t declare_variable;
	struct prototype *current_prototype;

	void *plugin_handle;
	void *codegen_plugin_handle;
	struct templatizer_plugin *plugin_data;
	struct templatizer_plugin *codegen_plugin_data;
	struct plugin_parameters_head plugin_parameters;
	struct plugin_variable_list_head *plugin_variables;
	int error;

	/* interpreter data */
	const char *script_path;
	const char *path_info;
	enum templatizer_format output_format;
	enum templatizer_compression output_compression;
	bool keep_alive;

	struct {
		tmpl_pool_t process;
		tmpl_pool_t connection;
	} pools;
};

#endif
