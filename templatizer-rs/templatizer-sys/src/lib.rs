pub mod templatizer {
    type OnElementCallback = fn();
    type CodegenTagStart = fn();
    type CodegenTagEnd = fn();

    pub struct Context {}

    pub struct Txn {}

    pub struct Sql {}

    pub struct TemplatizerCallbacks {
        /* Templatizer will manage the memory for the plugin. */
        /* This avoids memory leaks. */
        /* This is useful especially if Templatizer is run as a web server module. */
        pub malloc: fn(data: *mut Context, size: usize) -> *mut u8,
        pub free: fn(data: *mut Context, ptr: *mut u8),

        pub set_compression: fn(data: *mut Context, opt: isize),
        pub set_keep_alive: fn(data: *mut Context),
        pub send_header: fn(data: *mut Context, key: *const u8, value: *const u8),
        pub send_default_headers: fn(data: *mut Context),
        pub set_output_format: fn(data: *mut Context, fmt: isize),

        pub add_filler_text: fn(data: *mut Context, text: *const u8) -> isize,
        pub add_control_flow: fn(data: *mut Context, b: isize) -> isize, /* for conditionals */

        pub register_element_tag: fn(ctx: *mut Context, s: *const u8, cb: OnElementCallback),
        pub register_codegen_tag_start: fn(ctx: *mut Context, cb: CodegenTagStart),
        pub register_codegen_tag_end: fn(ctx: *mut Context, cb: CodegenTagEnd),

        pub exit: fn(ctx: *mut Context, status: isize),
        pub get_num_plugin_parameters: fn(ctx: *mut Context) -> isize,
        pub get_plugin_parameter: fn(ctx: *mut Context, index: isize, param_ptr: *mut *const u8, param_length: *mut usize) -> isize,

        /* Library agnostic API for accessing
         * a key-value data store.
         * This API can be used to make plugin
         * code portable among different
         * operatimg systems and plugin
         * languages. The only system dependent
         * code being the Templatizer plugin host
         * executable and the language runtime
         * of the plugin, if any. */
        pub storage_open: fn(path: *const u8),
        pub storage_begin_transaction:
          fn(txn: *mut *mut Txn),
        pub storage_commit_transaction:
          fn(txn: *mut Txn),
        pub storage_open_database:
          fn(txn: *mut Txn, dbi: *const isize),
        pub storage_close_database:
          fn(dbi: isize) -> isize,
        pub storage_get_string:
          fn(txn: *mut Txn,
           dbi: isize,
           key_id: isize,
           value: *mut *mut u8) -> isize,
        pub storage_get_integer:
          fn(txn: *mut Txn,
           dbi: isize,
           key_id: isize,
           value: *mut isize) -> isize,
        pub sql_connect: fn(connection: *mut Sql, s: *const u8) -> isize,
        pub sql_disconnect: fn(connection: *mut Sql) -> isize,
        pub sql_execute: fn(connection: *mut Sql) -> isize,
        pub sql_prepare: fn() -> isize,
        pub vm_define: fn() -> isize,
        pub vm_start: fn(name: *const u8) -> isize,
        pub vm_destroy: fn(name: *const u8) -> isize,
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
