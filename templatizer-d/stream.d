/* Copyright (C) 2022 Mateus de Lima Oliveira */

module stream;

import platform;

enum EOF = -1;
enum NULL = null;

private enum StreamError {
    ERROR_OK = 0,
    ELEMENT_OVERFLOW
};

private struct tmpl_stream {
    byte[] data;
    size_t position;
    bool read;
    bool write;
    int error;
}

public alias tmpl_stream_t = tmpl_stream *;

static tmpl_stream_t create_stream() @trusted
{
    import core.stdc.stdlib : calloc;
    tmpl_stream *s = cast(tmpl_stream *) calloc(tmpl_stream.sizeof, 1);
    return s;
}

extern(C)
tmpl_stream_t tmpl_fmemopen
    (void *buf, size_t size, const char *mode)
{
    tmpl_stream_t stream = create_stream();
    stream.data = null;
    stream.position = 0;
    stream.read = false;
    stream.write = false;
    stream.error = 0;
    string m = cast(immutable(char)[]) mode[0..strlen(mode)];
    foreach(c; m) {
        switch(c) {
            case 'r':
            stream.read = true;
            break;
            case 'w':
            stream.write = true;
            break;
            default:
            continue;
        }
    }
    stream.data = cast(byte[]) buf[0..size];
    return stream;
}

@system
static void free_stream(tmpl_stream_t stream)
{
    import core.memory : GC;
    GC.free(stream);
}

extern(C)
int tmpl_fclose(tmpl_stream_t stream)
{
    free_stream(stream);
    return 0;
}

extern(C)
int tmpl_fgetc(tmpl_stream_t stream)
{
    if (tmpl_feof(stream)) {
        return EOF;
    }
    int c = stream.data[stream.position];
    stream.position++;
    return c;
}

extern(C)
int tmpl_fputc(int c, tmpl_stream_t stream)
{
    if (tmpl_feof(stream)) {
        return EOF;
    }
    if (c > byte.max) {
        stream.error = StreamError.ELEMENT_OVERFLOW;
        return EOF;
    }
    stream.data[stream.position] = cast(byte) c;
    return c;
}

extern(C)
int tmpl_feof(tmpl_stream_t stream)
{
    if (stream.position >= stream.data.length) {
        if (stream.position >= stream.data.length) {
            stream.error = 1;
        }
        return 1;
    }
    return 0;
}

extern(C)
int tmpl_ferror(tmpl_stream_t stream)
{
    return stream.error;
}
