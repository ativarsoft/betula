/* Copyright (C) 2023 Mateus de Lima Oliveira */

/* Memory-safe XML parser */

@trusted
void createString(ref char[] s, size_t len)
{
    assert(s.ptr == null);
    import core.stdc.stdlib : calloc;
    auto buffer = cast(char *) calloc(len, char.sizeof);
    assert(buffer != null);
    s = buffer[0 .. len];
}

@trusted
void strdup(string s, ref char[] duplicate)
{
    createString(duplicate, s.length);
    for (int i = 0; i < s.length; i++)
        duplicate[i] = s[i];
}

/* This is useful for storing identifiers when they are larger
 * than the input buffer. */
struct String {
    char[] data;
}

@safe
size_t getLength(ref String s) {
    return s.data.length;
}

@system
size_t realloc(ref char[] s, size_t requestedLength) {
    assert(s.ptr != null);
    size_t previousLength = s.length;
    import core.stdc.stdlib : realloc;
    auto p = cast(char *) realloc(cast(void *) s.ptr, requestedLength * char.sizeof);
    assert(p != null);
    s = p[0 .. requestedLength];
    return previousLength;
}

@safe
char[] get(ref String s) {
    assert(s.data != null);
    return s.data;
}

@trusted
String *createString(string s) {
    import core.stdc.stdlib : calloc;
    auto cls = cast(String *) calloc(1, String.sizeof);
    strdup(s, cls.data);
    return cls;
}

@trusted
void appendString(String s, string tail)
{
    size_t previousLength = s.getLength();
    realloc(s.data, previousLength + tail.length);
    size_t j = 0;
    for (size_t i = 0; i < s.data.length; i++) {
        char c = tail[j];
        assert(c != '\0');
        s.data[i] = c;
        j++;
    }
}

struct StringCursor {
    string data;
    size_t pos;
}

@safe
char get(StringCursor *cursor) {
    size_t i = cursor.pos;
    cursor.pos++;
    return cursor.data[i];
}

@trusted
StringCursor *createStringCursor(string s)
{
    import core.stdc.stdlib : calloc;
    auto cls = cast(StringCursor *) calloc(1, StringCursor.sizeof);
    cls.data = s;
    cls.pos = 0;
    return cls;
}

struct XMLContext {
}

@safe
void xmlGetStartElement()
{
}

@safe
void xmlGetNode(XMLContext ctx, string buffer)
{
    StringCursor *cursor = createStringCursor(buffer);
    char c;
    c = get(cursor);
    switch (c) {
        case '<':
        xmlGetStartElement();
        break;

        default:
        assert(0);
        break;
    }
}


