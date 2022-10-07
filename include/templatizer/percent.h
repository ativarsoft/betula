#ifndef TMPL_PERCENT_H
#define TMPL_PERCENT_H

#include <stdio.h>

size_t tmpl_percent_decoded_len(char *in, size_t inputlen);
size_t tmpl_percent_decode_file(FILE *fin, FILE *fout, size_t *nbytes);
int tmpl_percent_decode_array
    (char *in, size_t inputlen,
     char *out, size_t outputlen,
     size_t *nbytes);

#endif
