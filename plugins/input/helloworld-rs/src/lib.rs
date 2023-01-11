/* Copyright (C) 2022 Mateus de Lima Oliveira */

extern crate templatizer_sys;
extern crate templatizer;
use templatizer::
{
    templatizer::
    {
        Context,
        TemplatizerCallbacks,
        TemplatizerPlugin,
        Templatizer
    }
};

extern "C"
fn init(data: *mut Context, cb: *const TemplatizerCallbacks) -> isize
{
    let tmpl = Templatizer::new(data, cb);
    tmpl.send_default_headers();
    tmpl.add_filler_text("Hello world from Rust!");
    return 0;
}

extern "C"
fn quit() {}

#[allow(non_upper_case_globals)]
#[no_mangle]
pub static templatizer_plugin_v1: TemplatizerPlugin = TemplatizerPlugin {
    init: init,
    quit: quit
};

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
