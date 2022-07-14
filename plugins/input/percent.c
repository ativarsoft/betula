#include <stdlib.h>
#include <stddef.h>

static int nibble(int c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return c - 'A';
	if (c >= 'a' && c <= 'f')
		return c-'a';
	return -1;
}

static int hex(const char *str)
{
	int n, tmp;

	tmp = nibble(str[0]);
	if (tmp < 0)
		return -1;
	n = tmp << 4;
	tmp = nibble(str[1]);
	if (tmp < 0)
		return -1;
	n += tmp;
	return n;
}

size_t tmpl_percent_encoded_len(const char *in)
{
	const char *p;
	size_t len = 0;

	/* Calculate decoded string length. */
	for (p = in; *p; p++) {
		if (*p == '%') {
			if (nibble(p[1]) < 0 || nibble(p[2]) < 0)
				return len;
			p += 2;
			len += 2;
		}
		len++;
	}
	return len;
}

size_t tmpl_percent_decoded_len(const char *in)
{
	const char *p;
	size_t len = 0;

	/* Calculate decoded string length. */
	for (p = in; *p; p++) {
		if (*p == '%') {
			if (nibble(p[1]) < 0 || nibble(p[2]) < 0)
				return len;
			p += 2;
		}
		len++;
	}
	return len;
}

/* In can be the same as out. */
size_t tmpl_percent_decode(const char *in, char *out)
{
	const char *p;
	size_t len = 0;

	for (p = in; *p; p++) {
		if (*p == '%') {
			*(out++) = hex(p + 1);
			p += 2;
		} else {
			*(out++) = *p;
		}
		len++;
	}
	return len;
}

void tmpl_percent_encode(char *in)
{
	/*int len;*/

	/* Calculate encoded string length. */
	/*malloc(len);*/
}
