/* Copyright (C) 2022 Mateus de Lima Oliveira */

#ifndef TMPL_STREAM_H
#define TMPL_STREAM_H

# ifndef USE_BUILTIN_STREAMS

#include <stdio.h>
#include <stddef.h>
#include <ctype.h>

typedef FILE *tmpl_stream_t;

#define tmpl_feof(stream) feof(stream)
#define tmpl_ferror(stream) ferror(stream)
#define tmpl_fmemopen(buf, size, mode) \
         fmemopen(buf, size, mode)
#define tmpl_fgetc(stream) fgetc(stream)
#define tmpl_fputc(c, stream) fputc(c, stream)
#define tmpl_fclose(stream) fclose(stream)

# else

struct tmpl_stream;

typedef struct tmpl_stream *tmpl_stream_t;

tmpl_stream_t tmpl_fmemopen
    (void *buf, size_t size, const char *mode);
int tmpl_fgetc(tmpl_stream_t stream);
int tmpl_fputc(int c, tmpl_stream_t stream);
int tmpl_fclose(tmpl_stream_t stream);

# endif

#endif
