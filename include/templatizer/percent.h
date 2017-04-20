#ifndef TMPL_PERCENT_H
#define TMPL_PERCENT_H

#include <stddef.h>

size_t percent_encoded_len(const char *in);
size_t percent_decoded_len(const char *in);
void percent_decode(const char *in, char *out);

#endif
