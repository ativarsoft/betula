/* Copyright (C) 2022 Mateus de Lima Oliveira */

#ifndef TMPL_PERCENT_H
#define TMPL_PERCENT_H

#include <templatizer/stream.h>

size_t tmpl_percent_decoded_len(const char *in, size_t inputlen);
size_t tmpl_percent_decode_file(tmpl_stream_t fin, tmpl_stream_t fout, size_t *nbytes);
int tmpl_percent_decode_array
    (char *in, size_t inputlen,
     char *out, size_t outputlen,
     size_t *nbytes);

#endif
