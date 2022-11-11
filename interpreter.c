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
#include <apr_pools.h>
#include <apr_strings.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <templatizer/compiler/compiler.h>
#include "memory.h"

static void print_html_escaped(const char *s, size_t len)
{
    int i;
    char c;
    for (i = 0; i < len; i++) {
        c = s[i];
        switch (c) {
            case '\"': puts("&quot;"); break;   //quotation mark
            case '\'': puts("&apos;"); break;   //apostrophe 
            case '&':  puts("&amp;");  break;   //ampersand
            case '<':  puts("&lt;");   break;   //less-than
            case '>':  puts("&gt;");   break;   //greater-than
            default:
            putchar(c);
        }
    }
}

static void dump_string(struct context *data)
{
	struct input *p;
	const char *s;

	if (TAILQ_EMPTY(data->input))
		return;
	p = TAILQ_FIRST(data->input);
	//fputs(p->data.filler_text, stdout);
	s= p->data.filler_text;
        print_html_escaped(s, strlen(s));
	TAILQ_REMOVE(data->input, p, entries);
	templatizer_free(data, p);
}

/* NOTE: not all keywords here are related to control flow anymore. */
bool is_control_flow_keyword(const XML_Char *el)
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
	bool just_print = false;

	for (s = n->data; (c = *s) != '\0'; s++) {
		if (just_print) {
			putchar(c);
			continue;
		}

		if (c == '\\')
			just_print = true;
		else if (c == '@')
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
			templatizer_free(data, p);
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

void print_list(tmpl_ctx_t data)
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
