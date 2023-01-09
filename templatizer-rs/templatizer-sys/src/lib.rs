pub mod templatizer {
    type OnElementCallback = fn();
    type CodegenTagStart = fn();
    type CodegenTagEnd = fn();

    #[repr(C)]
    pub struct Context;

    #[repr(C)]
    pub struct Txn;

    #[repr(C)]
    pub struct Sql;

    #[repr(C)]
    pub struct TemplatizerCallbacks {
        /* Templatizer will manage the memory for the plugin. */
        /* This avoids memory leaks. */
        /* This is useful especially if Templatizer is run as a web server module. */
        pub malloc:
            extern "C"
            fn(data: *mut Context, size: usize) -> *mut u8,
        pub free:
            extern "C"
            fn(data: *mut Context, ptr: *mut u8),

        pub set_compression:
            extern "C"
            fn(data: *mut Context, opt: isize),
        pub set_keep_alive:
            extern "C"
            fn(data: *mut Context),
        pub send_header:
            extern "C"
            fn(data: *mut Context, key: *const u8, value: *const u8),
        pub send_default_headers:
            extern "C"
            fn(data: *mut Context),
        pub set_output_format:
            extern "C"
            fn(data: *mut Context, fmt: isize),

        pub add_filler_text:
            extern "C"
            fn(data: *mut Context, text: *const u8) -> isize,
        pub add_control_flow:
            extern "C"
            fn(data: *mut Context, b: isize) -> isize, /* for conditionals */

        pub register_element_tag:
            extern "C"
            fn(ctx: *mut Context, s: *const u8, cb: OnElementCallback),
        pub register_codegen_tag_start:
            extern "C"
            fn(ctx: *mut Context, cb: CodegenTagStart),
        pub register_codegen_tag_end:
            extern "C"
            fn(ctx: *mut Context, cb: CodegenTagEnd),

        pub exit:
            extern "C"
            fn(ctx: *mut Context, status: isize),
        pub get_num_plugin_parameters:
            extern "C"
            fn(ctx: *mut Context) -> isize,
        pub get_plugin_parameter:
            extern "C"
            fn(ctx: *mut Context, index: isize, param_ptr: *mut *const u8, param_length: *mut usize) -> isize,

        /* Library agnostic API for accessing
         * a key-value data store.
         * This API can be used to make plugin
         * code portable among different
         * operatimg systems and plugin
         * languages. The only system dependent
         * code being the Templatizer plugin host
         * executable and the language runtime
         * of the plugin, if any. */
        pub storage_open:
          extern "C"
          fn(path: *const u8),
        pub storage_begin_transaction:
          extern "C"
          fn(txn: *mut *mut Txn),
        pub storage_commit_transaction:
          extern "C"
          fn(txn: *mut Txn),
        pub storage_open_database:
          extern "C"
          fn(txn: *mut Txn, dbi: *const isize),
        pub storage_close_database:
          extern "C"
          fn(dbi: isize) -> isize,
        pub storage_get_string:
          extern "C"
          fn(txn: *mut Txn,
           dbi: isize,
           key_id: isize,
           value: *mut *mut u8) -> isize,
        pub storage_get_integer:
          extern "C"
          fn(txn: *mut Txn,
           dbi: isize,
           key_id: isize,
           value: *mut isize) -> isize,
        pub sql_connect:
            extern "C" fn(connection: *mut Sql, s: *const u8) -> isize,
        pub sql_disconnect:
            extern "C" fn(connection: *mut Sql) -> isize,
        pub sql_execute:
            extern "C" fn(connection: *mut Sql) -> isize,
        pub sql_prepare:
            extern "C" fn() -> isize,
        pub vm_define:
            extern "C" fn() -> isize,
        pub vm_start:
            extern "C" fn(name: *const u8) -> isize,
        pub vm_destroy:
            extern "C" fn(name: *const u8) -> isize,
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
