#![no_std]

extern crate templatizer_sys;

pub mod templatizer {
    use templatizer_sys::templatizer::Context;
    use templatizer_sys::templatizer::TemplatizerCallbacks;

    pub struct Templatizer {
        ctx: *mut Context,
        cb: *const TemplatizerCallbacks,
    }

    impl Templatizer {
        pub fn new(ctx: *mut Context, cb: *const TemplatizerCallbacks) -> Self {
            Self {
                ctx: ctx,
                cb: cb,
            }
        }
        pub fn send_default_headers(&self) {
            unsafe {
                ((*self.cb).send_default_headers)(self.ctx);
            };
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
