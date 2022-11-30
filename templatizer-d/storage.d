module storage;

import templatizer;

extern(C) char *strndup(const char *s, size_t len);
extern(C) void free(void *p);

tmpl_ctx_t ctx;
tmpl_cb_t cb;

@safe
int storage_initialize_thread(tmpl_ctx_t a, tmpl_cb_t b)
{
    ctx = a;
    cb = b;
    return 0;
}

@safe
int storage_finalize_thread()
{
    ctx = null;
    cb = null;
    return 0;
}

extern(D)
@trusted
int storage_open(string path)
in (path != null)
{
    int rc = -1;
    char *s = strndup(path.ptr, path.length);
    rc = cb.storage_open(s);
    free(cast(void *) s);
    return rc;
}

extern(D)
@trusted
int storage_begin_transaction(ref tmpl_txn_t txn)
in (txn is null)
{
    int rc = -1;
    rc = cb.storage_begin_transaction(&txn);
    return rc;
}

extern(D)
@trusted
int storage_commit_transaction(tmpl_txn_t txn)
in (txn != null)
{
    int rc = -1;
    rc = cb.storage_commit_transaction(txn);
    return rc;
}

extern(D)
@trusted
int storage_open_database(tmpl_txn_t txn, ref tmpl_dbi_t dbi)
in (txn != null)
in (dbi is 0)
{
    int rc = -1;
    rc = cb.storage_open_database(txn, &dbi);
    return 0;
}

extern(D)
@trusted
int storage_close_database(tmpl_dbi_t dbi)
in (dbi != 0)
{
    int rc = -1;
    rc = cb.storage_close_database(dbi);
    return rc;
}

extern(D)
@trusted
int storage_get_string
   (tmpl_txn_t txn,
    tmpl_dbi_t dbi,
    int key_id,
    ref string value)
in (txn != null)
in (dbi != 0)
in (key_id > 0) /* key #0 is reserved */
{
    import core.stdc.string : strlen;
    char *p = null;
    int rc = -1;
    rc = cb.storage_get_string(txn, dbi, key_id, &p);
    if (rc == 0) {
        value = cast(string) p[0 .. strlen(p)];
    } else {
        value = null;
    }
    return rc;
}

extern(D)
@trusted
int storage_get_integer
   (tmpl_txn_t txn,
    tmpl_dbi_t dbi,
    int key_id,
    ref int value)
in (txn != null)
in (dbi != 0)
in (key_id > 0)
{
    int rc = -1;
    rc = cb.storage_get_integer(txn, dbi, key_id, &value);
    return rc;
}
