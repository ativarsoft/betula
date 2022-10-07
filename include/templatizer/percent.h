#ifndef PERCENT_H
#define PERCENT_H

#include <stdio.h>

size_t tmpl_percent_decode_file(FILE *fin, FILE *fout);
size_t tmpl_percent_decode_array(char *in, size_t inputlen, char *out, size_t outputlen);

#endif
#ifndef HTTP_COOKIE_H
#define HTTP_COOKIE_H

#include <stdio.h>

size_t tmpl_percent_decoded_len(char *in, size_t inputlen);
size_t tmpl_percent_decode_file(FILE *fin, FILE *fout, size_t *nbytes);
int tmpl_percent_decode_array
    (char *in, size_t inputlen,
     char *out, size_t outputlen,
     size_t *nbytes);

#endif
