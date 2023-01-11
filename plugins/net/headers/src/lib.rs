/* Copyright (C) 2023 Mateus de Lima Oliveira */

/* No standard library */
#[no_std]

extern crate templatizer_sys;
extern crate templatizer;

use templatizer_sys::templatizer::Context;
use templatizer_sys::templatizer::TemplatizerCallbacks;
use templatizer::templatizer::Templatizer;

static TMPL_FMT_HTML: isize = 1;
static TMPL_FMT_XHTML: isize = 2;

/* variables are considered to have 'static lifetime
 * bacause it they are live during all plugin execution time */
fn get_int_variable(name: &str) -> isize
{
    return TMPL_FMT_XHTML;
}

fn send_default_headers(tmpl: Templatizer)
{
    let output_format = get_int_variable("output_format");
    let content_type = match (output_format) {
        TMPL_FMT_HTML => {
            "text/html; charset=utf-8"
        },
        TMPL_FMT_XHTML => {
            "application/xhtml+xml; charset=utf-8"
        },
    };
    tmpl.send_header(data, "Content-Type", content_type);
    tmpl.send_header(data, "X-Powered-By", "Templatizer v1.0");
    tmpl.send_header(data, NULL, NULL);
}

extern "C"
fn init(ctx: *mut Context, cb: *const TemplatizerCallbacks)
{
    let tmpl = Templatizer::new(ctx, cb);
    send_default_headers(tmpl);
}

extern "C"
fn quit() {}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
