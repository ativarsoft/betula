/* Copyright (C) 2022 Mateus de Lima Oliveira */
#include <templatizer/percent.h>
#include <templatizer/stream.h>

/* Convert an hexadecimal nibble into an integer. */
static int nibble(int c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return (c - 'A') + 10;
	if (c >= 'a' && c <= 'f')
		return (c - 'a') + 10;
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

size_t tmpl_percent_decoded_len(const char *in, size_t inputlen)
{
	size_t len = 0;
	int hi, lo;
	FILE *f;

	/* Calculate decoded string length. */
	f = tmpl_fmemopen((void *) in, inputlen, "r");
	while (tmpl_feof(f) == 0) {
		if (tmpl_fgetc(f) == '%') {
			hi = tmpl_fgetc(f);
			lo = tmpl_fgetc(f);
			if (hex(hi, lo) < 0) {
				tmpl_fclose(f);
				return 0;
			}
			//len += 2;
		}
		len++;
	}
	tmpl_fclose(f);
	return len;
}

static int tmpl_percent_decode_byte(tmpl_stream_t fin)
{
	int c;
	int hi, lo;
	int ret = 0;

	c = tmpl_fgetc(fin);
	if (c == '%') {
		if ((hi = tmpl_fgetc(fin)) < 0)
			return -1;
		if ((lo = tmpl_fgetc(fin)) < 0)
			return -1;
		ret = hex(hi, lo);
	} else if (c == '+') {
		ret = ' ';
	} else {
#if 0
		/* Allow alphanumeric characters only. */
		if (isalnum(c) == 0)
			return -1;
#endif
		ret = c;
	}
	return ret;
}

/* In can be the same as out. */
int tmpl_percent_decode_file(tmpl_stream_t fin, tmpl_stream_t fout, size_t *nbytes)
{
	size_t len = 0;
	int c = 0;
	int ret = 0;

	while (tmpl_feof(fin) == 0) {
		if (tmpl_ferror(fin)) {
			ret = -1;
			goto out;
		}
		if ((c = tmpl_percent_decode_byte(fin)) < 0) {
			ret = -1;
			goto out;
		}
		if (tmpl_fputc(c, fout) < 0) {
			ret = -1;
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
	tmpl_stream_t fin, fout;

	if ((fin = tmpl_fmemopen(in, inputlen, "r")) == NULL)
		return 0;
	if ((fout = tmpl_fmemopen(out, outputlen, "w")) == NULL)
		return 0;
	ret = tmpl_percent_decode_file(fin, fout, nbytes);
	tmpl_fclose(fout);
	tmpl_fclose(fin);
	return ret;
}

static int tmpl_percent_encode_byte(tmpl_stream_t fout, int c)
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

int tmpl_percent_encode(tmpl_stream_t fin, tmpl_stream_t fout)
{
	int c;
	int ret = 0;

	while (tmpl_feof(fin) == 0) {
		if (tmpl_ferror(fin)) {
			ret = 1;
			goto out;
		}
		if (tmpl_ferror(fout)) {
			ret = 2;
			goto out;
		}
		c = tmpl_fgetc(fin);
		if (isalnum(c)) {
			if (tmpl_fputc(c, fout) == EOF) {
				ret = 3;
				goto out;
			}
		} else if (c == ' ') {
			if (tmpl_fputc(c, fout) == EOF) {
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
