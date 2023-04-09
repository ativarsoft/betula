/* Copyright (C) 2023 Mateus de Lima Oliveira */

/* No standard library */
#![no_std]

extern crate pollen_sys;

pub mod chacha20;

pub mod templatizer {
    pub type Context = pollen_sys::templatizer::Context;
    pub type TemplatizerCallbacks =
        pollen_sys::templatizer::TemplatizerCallbacks;
    pub type TemplatizerPlugin =
        pollen_sys::templatizer::TemplatizerPlugin;

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

        pub fn allocate_memory(&self, bytes: usize) -> *mut u8 {
            let f = unsafe  {(*self.cb).malloc };
            if (f as usize) == 0 {
                self.exit();
            }
            return f(self.ctx, bytes);
        }

        pub fn exit(&self) {
            unsafe {
                let f = (*self.cb).exit;
                if (f as usize) == 0 {
                   loop {}
                }
                f(self.ctx, 1);
            }
        }

        pub fn send_default_headers(&self) {
            unsafe {
                let f = (*self.cb).send_default_headers;
		if (f as usize) == 0 {
                    self.exit();
		}
                f(self.ctx);
            };
        }

        pub fn add_filler_text(&self, s: &str) -> isize {
            let rc = unsafe {
                let f = (*self.cb).add_filler_text;
                if (f as usize) == 0 {
                    self.exit();
                }
                f(self.ctx, s.as_ptr())
            };
            return rc;
        }

        pub fn add_control_flow(&self, b: isize) -> isize {
            let rc = unsafe {
                let f = (*self.cb).add_control_flow;
                if (f as usize) == 0 {
                    self.exit();
                }
                f(self.ctx, b)
            };
            return rc;
        }

        unsafe fn strndup(&self, ptr: *const u8, length: usize) -> *mut u8 {
            let f = (*self.cb).strndup;
            let ret = f(self.ctx, ptr, length);
            return ret;
        }
    }

    pub struct String {
        ptr: *mut u8,
        length: usize,
    }

    impl String {
        pub fn new() -> Self {
            let string = String {
                ptr: 0 as *mut u8,
                length: 0,
            };
            return string;
        }

        pub fn new_from_str(tmpl: &Templatizer, slice: &str) -> Self {
            let ptr = unsafe {
                tmpl.strndup(slice.as_ptr(), slice.len())
            };
            let string = String {
                ptr: ptr,
                length: 0,
            };
            return string;
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }

    fn chacha20_quarter_round() {
        assert_eq!(chacha20::chacha20::ChaCha20::test_quarter_round(), true);
    }

    fn chacha20_test1() {
        let ptr = 0 as *mut templatizer::Templatizer;
        let ctx = unsafe { &mut *ptr };
        assert_eq!(chacha20::chacha20::ChaCha20::test1(ctx), true);
    }
}

