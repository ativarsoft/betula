/* Copyright (C) 2017-2023 Mateus de Lima Oliveira */

struct Context {
};

struct NodeStart {
    string el;
    string[] attr;
    size_t num_attributes;
};

struct NodeEnd {
    string el;
};

struct NodeCharacterData {
    string data;
};

@trusted
static void putchar(int c)
{
    import stdio = core.stdc.stdio : putchar;
    stdio.putchar(c);
}

@safe
static void puts(string s)
{
    foreach (char c; s) {
        if (c == '\0')
            break;
        putchar(c);
    }
}

@safe
static int strcmp(string a, string b)
{
    char c1, c2;
    for (int i = 0; i < a.length && i < b.length; i++) {
        c1 = a[i]; c2 = b[i];
        if (c1 == '\0' || c2 == '\0')
            break;
        if (c1 == c2)
            continue;
        if (c1 < c2)
            return -1;
        else
            return 1;
    }
    return 0;
}

@safe
static void print_html_escaped(string s)
{
    foreach (char c; s) {
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

@safe
static void dump_string(ref Context data)
{
	/*struct input *p;
	const char *s;

	if (TAILQ_EMPTY(data->input))
		return;
	p = TAILQ_FIRST(data->input);
	//fputs(p->data.filler_text, stdout);
	s= p->data.filler_text;
        print_html_escaped(s, strlen(s));
	TAILQ_REMOVE(data->input, p, entries);
	templatizer_free(data, p);*/
}

// NOTE: not all keywords here are related to control flow anymore.
@safe static bool is_control_flow_keyword(string el)
{
	if (strcmp("if", el) == 0
		|| strcmp("swhile", el) == 0
		|| strcmp("ewhile", el) == 0)
		return true;
	return false;
}

@safe
static void print_start_node(ref Context data, ref NodeStart n)
{
    int i;

    if (strcmp("templatizer", n.el) == 0)
        return;
    if (is_control_flow_keyword(n.el))
        return;
    putchar('<');
    puts(n.el);
    if (n.attr) {
        for (i = 0; i < n.num_attributes; i += 2) {
            if (strcmp("@", n.attr[i + 1]) == 0) {
                putchar(' ');
                puts(n.attr[i]);
                putchar('=');
                putchar('\"');
                dump_string(data);
                putchar('\"');
            } else {
                putchar(' ');
                puts(n.attr[i]);
                putchar('=');
                putchar('\"');
                puts(n.attr[i + 1]);
                putchar('\"');
            }
        }
    }
    putchar('>');
}

@safe
static void print_end_node(ref NodeEnd n)
{
    if (strcmp("templatizer", n.el) == 0)
        return;
    if (is_control_flow_keyword(n.el))
        return;
    putchar('<');
    putchar('/');
    puts(n.el);
    putchar('>');
}

@safe
static void print_character_data_node(ref Context data, ref NodeCharacterData n)
{
    string s = n.data;
    bool just_print = false;

    foreach (char c; s) {
        if (c == '\0')
            break;
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

/*static enum control_flow print_node(struct context *data, struct node *n)
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
			if (p->data.control_flow == TMPL_TRUE)
				r = NEXT_INSTRUCTION;
			else
				r = JMP_FOWARD;
			TAILQ_REMOVE(data->input, p, entries);
			templatizer_free(data, p);
			return r;
		}
		print_start_node(data, &n->data.start);
		break;
	case NODE_END:
		if (n->data.end.jmp) {
			if (n->data.end.conditional_jmp == false)
				return JMP_BACKWARD;
			if (TAILQ_EMPTY(data->input)) {
				fprintf(stderr, "missing input for tag that requires input\n");
				exit(1);
			}
			p = TAILQ_FIRST(data->input);
			if (p->data.control_flow == TMPL_TRUE)
				r = JMP_BACKWARD;
			else
				r = NEXT_INSTRUCTION;
			TAILQ_REMOVE(data->input, p, entries);
			templatizer_free(data, p);
			return r;
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
}*/
