mod templatizer {
    struct Templatizer {
        struct templatizer_callbacks {
	/* Templatizer will manage the memory for the plugin. */
	/* This avoids memory leaks. */
	/* This is useful especially if Templatizer is run as a web server module. */
	malloc: fn(struct context *data, size_t size),
	free: fn(struct context *data, void *ptr),

	set_compression: fn(struct context *data, enum templatizer_compression opt);
	set_keep_alive: fn(struct context *data);
	send_header: fn(struct context *data, const char *key, const char *value);
	send_default_headers: fn(struct context *data);
	set_output_format: fn(struct context *data, enum templatizer_format fmt);

	add_filler_text: fn(struct context *data, const char *text) -> isize,
	add_control_flow: fn(struct context *data, int b) -> isize, /* for conditionals */

	int (*register_element_tag)(tmpl_ctx_t ctx, char *s, on_element_callback_t cb);
	int (*register_codegen_tag_start)(tmpl_ctx_t ctx, tmpl_codegen_tag_start_t cb);
	int (*register_codegen_tag_end)(tmpl_ctx_t ctx, tmpl_codegen_tag_end_t cb);

	exit: fn(tmpl_ctx_t ctx, int status),
	get_num_plugin_parameters: fn(tmpl_ctx_t ctx) -> isize,
	get_plugin_parameter: fn(tmpl_ctx_t ctx, int index, const char **param_ptr, size_t *param_length) -> isize,

	/* Library agnostic API for accessing
	 * a key-value data store.
	 * This API can be used to make plugin
	 * code portable among different
	 * operatimg systems and plugin
	 * languages. The only system dependent
	 * code being the Templatizer plugin host
	 * executable and the language runtime
	 * of the plugin, if any. */
	int (*storage_open)(const char *path),
	int (*storage_begin_transaction)
	  (tmpl_txn_t *txn),
	int (*storage_commit_transaction)
	  (tmpl_txn_t txn),
	int (*storage_open_database)
	  (tmpl_txn_t txn, tmpl_dbi_t *dbi),
	int (*storage_close_database)
	  (tmpl_dbi_t dbi) -> isize,
	int (*storage_get_string)
	  (tmpl_txn_t txn,
	   tmpl_dbi_t dbi,
	   int key_id,
	   char **value) -> isize,
	storage_get_integer
	  (tmpl_txn_t txn,
	   tmpl_dbi_t dbi,
	   int key_id,
	   int *value) -> isize,
        sql_connect: fn(tmpl_sql_t *connection, const char *s) -> isize,
        sql_disconnect: fn(tmpl_sql_t *connection) -> isize,
        sql_execute: fn(tmpl_sql_t *connection) -> isize,
        sql_prepare: fn() -> isize,
        vm_define: fn() -> isize,
        vm_start: fn(const char *name) -> isize,
        vm_destroy: fn(const char *name) -> isize,
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
