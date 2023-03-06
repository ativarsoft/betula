#ifndef TMPL_STORAGE
#define TMPL_STORAGE

typedef enum {
    TMPL_INODE_DIR,
    TMPL_INODE_STRING,
    TMPL_INODE_INTEGER
} inode_type_t;

typedef struct {
    int next_id;
    char *name;
    inode_type_t type;
} tmpl_dir_t;

int storage_initialize();
int storage_finalize();
int storage_open(const char *path);
int storage_begin_transaction
  (tmpl_txn_t *txn);
int storage_commit_transaction
  (tmpl_txn_t txn);
int storage_open_database
  (tmpl_txn_t txn, tmpl_dbi_t *dbi);
int storage_close_database
  (tmpl_dbi_t dbi);
int storage_get_string
  (tmpl_txn_t txn,
   tmpl_dbi_t dbi,
   int key_id,
   char **value);
int storage_get_integer
  (tmpl_txn_t txn,
   tmpl_dbi_t dbi,
   int key_id,
   int *value);

#endif
