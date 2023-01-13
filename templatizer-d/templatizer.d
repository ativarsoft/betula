/* Copyright (C) 2022 Mateus de Lima Oliveira */

module templatizer;

public import runtime;
public import query;
public import stream;
public import percent;
public import storage;

extern(C) {

enum TMPL_NOT_JMP = 0;
enum TMPL_JMP = 1;
enum IF_TRUE = TMPL_NOT_JMP;
enum IF_FALSE = TMPL_JMP;
enum SWHILE_TRUE = TMPL_NOT_JMP;
enum SWHILE_FALSE = TMPL_JMP;

struct context;
alias tmpl_ctx_t = context *;
alias tmpl_cb_t = templatizer_callbacks *;

struct MDB_txn {};
alias MDB_dbi = int;

alias tmpl_txn_t = MDB_txn *;
alias tmpl_dbi_t = MDB_dbi;

enum templatizer_compression {
	TMPL_Z_PLAIN,
	TMPL_Z_GZIP,
	TMPL_Z_DEFLATE
};

enum templatizer_format {
	TMPL_FMT_HTML,
	TMPL_FMT_XHTML
};

alias on_element_callback_t =
int function
    (tmpl_ctx_t ctx);

alias tmpl_codegen_tag_start_t =
int function
    (tmpl_ctx_t ctx,
     const char *el,
     const char **attr);

alias tmpl_codegen_tag_end_t =
int function
    (tmpl_ctx_t ctx,
     const char *el);

alias tmpl_codegen_control_flow_start_t =
int function
    (tmpl_ctx_t ctx);

alias tmpl_codegen_control_flow_end_t =
int function
    (tmpl_ctx_t ctx,
     const char *label);

struct templatizer_callbacks {
	/* Templatizer will manage the memory for the plugin. */
	/* This avoids memory leaks. */
	/* This is useful especially if Templatizer is run as a web server module. */
	void *function(tmpl_ctx_t data, size_t size) malloc;
	void function(tmpl_ctx_t data, void *ptr) free;

	/* array functions */
	const(char) *function(tmpl_ctx_t ctx, const(char) *ptr, size_t length) strndup;

	void function(tmpl_ctx_t data, templatizer_compression opt) set_compression;
	void function(tmpl_ctx_t data) set_keep_alive;
	void function(tmpl_ctx_t data, const char *key, const char *value) send_header;
	void function(tmpl_ctx_t data) send_default_headers;
	void function(tmpl_ctx_t data, templatizer_format fmt) set_output_format;

	int function(tmpl_ctx_t data, const char *text, size_t max_length) add_filler_text;
	int function(tmpl_ctx_t data, int b) add_control_flow; /* for conditionals */

	int function(tmpl_ctx_t ctx, char *s, on_element_callback_t cb) register_element_tag;
	int function(tmpl_ctx_t ctx, tmpl_codegen_tag_start_t cb) register_element_tag_start;
	int function(tmpl_ctx_t ctx, tmpl_codegen_tag_end_t cb) register_element_tag_end;

	void function(tmpl_ctx_t ctx, int status) exit;

	int function(const char *path) storage_open;
        int function(tmpl_txn_t *txn) storage_begin_transaction;
        int function(tmpl_txn_t txn) storage_commit_transaction;
        int function(tmpl_txn_t txn, tmpl_dbi_t *dbi) storage_open_database;
        int function(tmpl_dbi_t dbi) storage_close_database;
        int function
          (tmpl_txn_t txn,
           tmpl_dbi_t dbi,
           int key_id,
           char **value)
           storage_get_string;
        int function
          (tmpl_txn_t txn,
           tmpl_dbi_t dbi,
           int key_id,
           int *value)
           storage_get_integer;
};

struct templatizer_plugin {
	int function(tmpl_ctx_t data, tmpl_cb_t cb) init;
	void function() quit;
};

} /* extern(C) */

tmpl_ctx_t ctx;
tmpl_cb_t cb;

@trusted
void tmpl_send_default_headers()
{
	assert(ctx != null);
	assert(cb != null);
	cb.send_default_headers(ctx);
}

@trusted
void tmpl_add_filler_text(string s)
{
	assert(ctx != null);
	assert(cb != null);
	cb.add_filler_text(ctx, s.ptr, s.length);
}
