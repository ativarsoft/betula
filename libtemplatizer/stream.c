#include <templatizer/stream.h>

#ifdef TMPL_USE_BUILTIN_STREAMS

tmpl_stream_t tmpl_fmemopen
    (void *buf, size_t size, const char *mode)
{
	return NULL;
}

int tmpl_fclose(tmpl_stream_t stream)
{
	return 0;
}

int tmpl_fgetc(tmpl_stream_t stream)
{
	return 0;
}

int tmpl_fputc(int c, tmpl_stream_t stream)
{
	return 0;
}

#endif
