/* Copyright (C) 2017-2023 Mateus de Lima Oliveira */

import list_d;

enum TMPL_TRUE = 1;
enum TMPL_FALSE = 0;

enum ControlFlow {
    NEXT_INSTRUCTION,
    JMP_FOWARD,
    JMP_BACKWARD
};

struct NodeStart {
    string el;
    string[] attr;
    size_t num_attributes;
    bool jmp;
};

struct NodeEnd {
    string el;
    bool jmp;
    bool conditional_jmp;
};

struct NodeCharacterData {
    string data;
};

enum NodeType {
    NODE_START,
    NODE_END,
    NODE_CHARACTER_DATA
};

union NodeData {
    NodeStart start;
    NodeEnd end;
    NodeCharacterData character_data;
};

union Node {
    NodeType type;
    NodeData data;
};

alias NodeList = List!Node;

enum InputType {
    INPUT_FILLER_TEXT,
    INPUT_CONTROL_FLOW
};

struct Input {
    InputType type;
    union InputData {
        string filler_text;
        bool control_flow;
    };
    InputData data;
};

alias InputList = List!Input;

struct Context {
    NodeList nodes;
    InputList input;
};

@trusted
static void putchar(const(int) c)
{
    import stdio = core.stdc.stdio : putchar;
    stdio.putchar(c);
}

@live @trusted
static void puts(scope const(string) s)
{
    int c = 0;
    for (size_t i = 0; s[i]; i++) {
        c = s[i];
        if (c == '\0')
            break;
        putchar(c);
    }
}

@live @safe
static int strcmp(scope string a, scope string b)
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

@live @safe
static void print_html_escaped(scope string s)
{
    char c = '\0';
    for (int i = 0; s[i]; i++) {
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

@trusted
string getFillerText(scope return const(Input) *p)
{
    assert(p.type == InputType.INPUT_FILLER_TEXT);
    return p.data.filler_text;
}

@live @safe
static void dump_string(scope Context *data)
{
    if (data.input.isEmpty())
        return;
    scope const(Input) *p = data.input.getFirst();
    //fputs(p->data.filler_text, stdout);
    scope string s = p.getFillerText();
    print_html_escaped(s);
    data.input.removeHead();
}

// NOTE: not all keywords here are related to control flow anymore.
@live @safe
static bool is_control_flow_keyword(scope string el)
{
	if (strcmp("if", el) == 0
		|| strcmp("swhile", el) == 0
		|| strcmp("ewhile", el) == 0)
		return true;
	return false;
}

@live @safe
static void print_start_node(scope Context *data, scope NodeStart *n)
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

@live @safe
static void print_end_node(scope const(NodeEnd) *n)
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

@live @safe
static void print_character_data_node(scope Context *data, scope NodeCharacterData *n)
{
    scope string s = n.data;
    bool just_print = false;
    char c = '\0';

    for (int i = 0; i < s.length; i++) {
        c = s[i];
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

@trusted
static void printError(string s)
{
    import core.stdc.stdio : fputc, stderr;
    foreach(char c; s) {
        if (c == '\0')
            break;
        fputc(c, stderr);
    }
}

@trusted void exit(int errorCode)
{
    import stdlib = core.stdc.stdlib : exit;
    stdlib.exit(errorCode);
}

@trusted NodeStart *getNodeStart(scope return Node *n)
{
    assert(n.type == NodeType.NODE_START);
    return &n.data.start;
}

@trusted NodeEnd *getNodeEnd(scope return Node *n)
{
    assert(n.type == NodeType.NODE_END);
    return &n.data.end;
}

@trusted NodeCharacterData *getNodeCharacterData(scope return Node *n)
{
    assert(n.type == NodeType.NODE_CHARACTER_DATA);
    return &n.data.character_data;
}

@live @safe
static ControlFlow print_node(scope Context *data, scope Node *n)
{
    ControlFlow r;

    switch (n.type) {
        case NodeType.NODE_START:
        scope NodeStart *startNode = getNodeStart(n);
        if (startNode.jmp) {
            if (data.input.isEmpty()) {
                printError("missing input for tag that requires input\n");
                exit(1);
            }
            scope const(Input) *p = data.input.getFirst();
            if (p.data.control_flow == TMPL_TRUE)
                r = ControlFlow.NEXT_INSTRUCTION;
            else
                r = ControlFlow.JMP_FOWARD;
            data.input.removeHead();
            return r;
        }
        scope NodeStart *nodeStart = getNodeStart(n);
        print_start_node(data, nodeStart);
        break;

        case NodeType.NODE_END:
        scope NodeEnd *endNode = getNodeEnd(n);
        if (endNode.jmp) {
            if (endNode.conditional_jmp == false)
                return ControlFlow.JMP_BACKWARD;
            if (data.input.isEmpty()) {
                printError("missing input for tag that requires input\n");
                exit(1);
            }
            scope const(Input) *p = data.input.getFirst();
            if (p.data.control_flow == TMPL_TRUE)
                r = ControlFlow.JMP_BACKWARD;
            else
                r = ControlFlow.NEXT_INSTRUCTION;
            data.input.removeHead();
            return r;
        }
        //NodeEnd endNode = getNodeEnd(n);
        print_end_node(endNode);
        break;

        case NodeType.NODE_CHARACTER_DATA:
        scope NodeCharacterData *characterDataNode = getNodeCharacterData(n);
        print_character_data_node(data, characterDataNode);
        break;

        default:
        printError("Error: unknown node type.\n");
        assert(0);
        break;
    }
    return ControlFlow.NEXT_INSTRUCTION;
}

@trusted
void flush()
{
    import core.stdc.stdio : fflush, stdout;
    fflush(stdout);
}

@safe
void print_list(scope Context *data)
{
    auto p = data.nodes.createCursor();
    while (p.isValid()) {
        scope Node *n = p.get();
        switch (print_node(data, n)) {
            case ControlFlow.JMP_FOWARD:
            //p = TAILQ_NEXT(p->data.start.jmp, entries);
            break;
            case ControlFlow.JMP_BACKWARD:
            //p = p.data.end.jmp;
            break;
            default:
            p.next();
            break;
        }
        flush();
    }
}

