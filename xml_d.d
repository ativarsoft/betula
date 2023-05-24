/* Copyright (C) 2023 Mateus de Lima Oliveira */

/* Memory-safe XML parser */

@trusted
char[] strdup(string s)
{
    import core.stdc.stdlib : calloc;
    auto buffer = cast(char *) calloc(s.length, char.sizeof);
    assert(buffer != null);
    auto duplicate = cast(char[]) buffer[0 .. s.length];
    for (int i = 0; i < s.length; i++)
        duplicate[i] = s[i];
    return duplicate;
}

/* This is useful for storing identifiers when they are larger
 * than the input buffer. */
extern(C++) final class String {
    extern(C) char[] data;

    @safe
    size_t getLength() {
        return data.length;
    }

    @system
    size_t realloc(size_t requestedLength) {
        size_t previousLength = data.length;
        import core.stdc.stdlib : realloc;
        auto p = cast(char *) realloc(cast(void *) data, requestedLength);
        assert(p != null);
        data = p[0 .. requestedLength];
        return previousLength;
    }
}

@safe
char[] get(ref String s) {
    assert(s.data != null);
    return s.data;
}

@trusted
String createString(string s) {
    import core.stdc.stdlib : calloc;
    auto cls = cast(String) calloc(1, String.sizeof);
    cls.data = strdup(s);
    return cls;
}

@trusted
void appendString(String s, string tail)
{
    size_t previousLength = s.getLength();
    s.realloc(previousLength + tail.length);
    size_t j = 0;
    for (size_t i = 0; i < s.data.length; i++) {
        char c = tail[j];
        assert(c != '\0');
        s.data[i] = c;
        j++;
    }
}

extern(C++) final class StringCursor {
    extern(C) string data;
    extern(C) size_t pos;
}

@safe
char get(StringCursor cursor) {
    size_t i = cursor.pos;
    cursor.pos++;
    return cursor.data[i];
}

@trusted
StringCursor createStringCursor(string s)
{
    import core.stdc.stdlib : calloc;
    auto cls = cast(StringCursor) calloc(1, StringCursor.sizeof);
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
    StringCursor cursor = createStringCursor(buffer);
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


