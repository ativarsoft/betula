#include <templatizer/percent.h>
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
		return EOF;
	n = tmp << 4;
	tmp = nibble(lo);
	if (tmp < 0)
		return EOF;
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

static int tmpl_percent_decode_byte(FILE *fin)
{
	int c;
	int hi, lo;
	int ret = 0;

	c = fgetc(fin);
	if (c == '%') {
		if ((hi = fgetc(fin)) == EOF)
			return EOF;
		if ((lo = fgetc(fin)) == EOF)
			return EOF;
		ret = hex(hi, lo);
	} else if (c == '+') {
		ret = ' ';
	} else {
		if (isalnum(c) == 0)
			return EOF;
		ret = c;
	}
	return ret;
}

/* In can be the same as out. */
size_t tmpl_percent_decode_file(FILE *fin, FILE *fout, size_t *nbytes)
{
	size_t len = 0;
	int c = 0;
	int ret = 0;

	while (feof(fin) == 0) {
		if (ferror(fin)) {
			ret = EOF;
			goto out;
		}
		if ((c = tmpl_percent_decode_byte(fin)) == EOF) {
			ret = EOF;
			goto out;
		}
		if (fputc(c, fout) == EOF) {
			ret = EOF;
			goto out;
		}
		len++;
	}
out:
	*nbytes = len;
	return ret;
}

int tmpl_percent_decode_array
    (char *in, size_t inputlen,
     char *out, size_t outputlen,
     size_t *nbytes)
{
	int ret = 0;
	FILE *fin, *fout;

	if ((fin = fmemopen(in, inputlen, "r")) == NULL)
		return 0;
	if ((fout = fmemopen(out, outputlen, "w")) == NULL)
		return 0;
	ret = tmpl_percent_decode_file(fin, fout, nbytes);
	fclose(fout);
	fclose(fin);
	return ret;
}

static int tmpl_percent_encode_byte(FILE *fout, int c)
{
	char byte;
	int nibble;

	byte = (char) c;
	/* check if there was an underfow during conversion */
	if (byte != c)
		return 1;

	nibble = ((c >> 4) & 0x0F) + '0';
	if (fputc(nibble, fout))
		return 2;
	nibble = (c & 0x0F) + '0';
	if (fputc(nibble, fout))
		return 2;

	return 0;
}

int tmpl_percent_encode(FILE *fin, FILE *fout)
{
	int c;
	int ret = 0;

	while (feof(fin) == 0) {
		if (ferror(fin)) {
			ret = 1;
			goto out;
		}
		if (ferror(fout)) {
			ret = 2;
			goto out;
		}
		c = fgetc(fin);
		if (isalnum(c)) {
			if (fputc(c, fout) == EOF) {
				ret = 3;
				goto out;
			}
		} else if (c == ' ') {
			if (fputc(c, fout) == EOF) {
				ret = 3;
				goto out;
			}
		} else {
			if (tmpl_percent_encode_byte(fout, c)) {
				ret = 4;
				goto out;
			}
		}
	}
out:
	return ret;
}
