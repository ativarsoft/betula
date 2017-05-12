#ifndef TMPL_PERCENT_H
#define TMPL_PERCENT_H

#include <stddef.h>

size_t tmpl_percent_encoded_len(const char *in);
size_t tmpl_percent_decoded_len(const char *in);
size_t tmpl_percent_decode(const char *in, char *out);

#endif
