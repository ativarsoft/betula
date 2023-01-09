/* Copyright (C) 2022 Mateus de Lima Oliveira */

extern crate templatizer_sys;
extern crate templatizer;
use templatizer_sys::templatizer::{Context,TemplatizerCallbacks};
use templatizer::templatizer::Templatizer;

extern "C"
fn init(data: *mut Context, cb: *const TemplatizerCallbacks) -> isize
{
    let tmpl = Templatizer::new(data, cb);
    tmpl.send_default_headers();
    return 0;
}

extern "C"
fn quit() {}

#[repr(C)]
pub struct TemplatizerPlugin {
    init: extern "C" fn(data: *mut Context, cb: *const TemplatizerCallbacks) -> isize,
    quit: extern "C" fn(),
}

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
