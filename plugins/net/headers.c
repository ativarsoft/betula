#include <pollen/pollen.h>

#define VERSION "1.0"

void send_default_headers(data, cb)
tmpl_ctx_t data;
tmpl_cb_t cb;
{
	const char *content_type = "text/plain; charset=utf-8";
	int output_format = cb->get_int_variable(data, "output_format");

	switch (output_format) {
		case TMPL_FMT_HTML:
		content_type = "text/html; charset=utf-8";
		break;
		case TMPL_FMT_XHTML:
		content_type = "application/xhtml+xml; charset=utf-8";
		break;
	}
	cb->send_header(data, "Content-Type", content_type);
	cb->send_header(data, "X-Powered-By", "Templatizer " VERSION);
	cb->send_header(data, NULL, NULL);
}

static int init(tmpl_ctx_t data, struct pollen_callbacks *cb)
{
	send_default_headers(data, cb);
	return 0;
}

static void quit(void) {}

const tmpl_plugin_record_t templatizer_plugin_v1 = {
	&init,
	&quit
};
