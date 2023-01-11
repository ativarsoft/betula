/* Copyright (C) 2023 Mateus de Lima Oliveira */

extern crate templatizer_sys;

pub mod templatizer {
    pub type Context = templatizer_sys::templatizer::Context;
    pub type TemplatizerCallbacks =
        templatizer_sys::templatizer::TemplatizerCallbacks;
    pub type TemplatizerPlugin =
        templatizer_sys::templatizer::TemplatizerPlugin;

    pub struct Templatizer {
        ctx: *mut Context,
        cb: *const TemplatizerCallbacks,
    }

    impl Templatizer {
        pub fn new(ctx: *mut Context, cb: *const TemplatizerCallbacks) -> Self {
            return Self {
                ctx: ctx,
                cb: cb,
            }
        }

        pub fn send_default_headers(&self) {
            unsafe {
                ((*self.cb).send_default_headers)(self.ctx);
            };
        }

        pub fn add_filler_text(&self, s: &str) -> isize {
            let rc = unsafe {
                ((*self.cb).add_filler_text)(self.ctx, s.as_ptr())
            };
            return rc;
        }

        pub fn add_control_flow(&self, b: isize) -> isize {
            let rc = unsafe {
                ((*self.cb).add_control_flow)(self.ctx, b)
            };
            return rc;
        }
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
