#include <stdio.h>
#include <stddef.h>
#include <ctype.h>

/* Convert an hexadecimal nibble into an integer. */
static int nibble(int c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return c - 'A';
	if (c >= 'a' && c <= 'f')
		return c - 'a';
	return -1;
}

/* Convert hexadeciman byte into integer.
 * Take the high and the low nibble as arguments. */
static int hex(int hi, int lo)
{
	int n, tmp;

	tmp = nibble(hi);
	if (tmp < 0)
		return -1;
	n = tmp << 4;
	tmp = nibble(lo);
	if (tmp < 0)
		return -1;
	n |= tmp;
	return n;
}

size_t tmpl_percent_decoded_len(char *in, size_t inputlen)
{
	size_t len = 0;
	int hi, lo;
	FILE *f;

	/* Calculate decoded string length. */
	f = fmemopen(in, inputlen, "r");
	while (feof(f) == 0) {
		if (fgetc(f) == '%') {
			hi = fgetc(f);
			lo = fgetc(f);
			if (hex(hi, lo) < 0) {
				return 0;
				fclose(f);
			}
			len += 2;
		}
		len++;
	}
	fclose(f);
	return len;
}

/* In can be the same as out. */
size_t tmpl_percent_decode(char *in, size_t inputlen, char *out, size_t outputlen)
{
	size_t len = 0;
	int c = 0;
	int hi, lo;
	int ret = 0;
	FILE *fin, *fout;

	fin = fmemopen(in, inputlen, "r");
	if (fin == NULL)
		return 1;
	fout = fmemopen(out, outputlen, "w");
	if (fout == NULL)
		return 2;
	while (feof(fin) == 0) {
		c = fgetc(fin);
		if (c == '%') {
			hi = fgetc(fin);
			lo = fgetc(fin);
			c = hex(hi, lo);
			if (c < 0) {
				ret = 3;
				goto out;
			}
			fputc(c, fout);
		} else {
			if (isalnum(c) == 0) {
				ret = 4;
				goto out;
			}
			fputc(c, fout);
		}
		len++;
	}
out:
	fclose(fout);
	fclose(fin);
	return ret;
}

void tmpl_percent_encode(char *in)
{
}
