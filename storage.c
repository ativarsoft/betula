/* Copyright (C) 2022 Mateus de Lima Oliveira */

#include "config.h"

#ifdef USE_STORAGE

#include <templatizer.h>
#include <templatizer/compiler/compiler.h>
#include <threads.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <lmdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdbool.h>
#include "storage.h"

static thread_local tmpl_ctx_t dav_ctx;
static thread_local struct templatizer_callbacks *dav_cb;

typedef MDB_txn *tmpl_txn_t;
typedef MDB_dbi tmpl_dbi_t;

static thread_local MDB_env *env = NULL;

int storage_open(const char *path)
{
  int rc;
  rc = mdb_env_open(env, path, 0, 0664);
  return rc != MDB_SUCCESS;
}

int storage_initialize()
{
  int rc;

  assert(env == NULL);
  rc = mdb_env_create(&env);
  return rc != MDB_SUCCESS;
}

int storage_finalize()
{
  assert(env != NULL);
  mdb_env_close(env);
  return 0;
}

int storage_begin_transaction(tmpl_txn_t *txn)
{
  int rc;
  assert(env != NULL);
  rc = mdb_txn_begin(env, NULL, 0, txn);
  return rc != MDB_SUCCESS;
}

int storage_commit_transaction(tmpl_txn_t txn)
{
  int rc;
  assert(env != NULL);
  rc = mdb_txn_commit(txn);
  return rc != MDB_SUCCESS;
}

int storage_abort_transaction(tmpl_txn_t txn)
{
  assert(env != NULL);
  mdb_txn_abort(txn);
  return 0;
}

int storage_open_database
  (tmpl_txn_t txn, tmpl_dbi_t *dbi)
{
  int rc;
  assert(env != NULL);
  rc = mdb_open(txn, NULL, 0, dbi);
  return rc != MDB_SUCCESS;
}

int storage_close_database(tmpl_dbi_t dbi)
{
  assert(env != NULL);
  mdb_close(env, dbi);
  return 0;
}

int storage_get_string
  (tmpl_txn_t txn,
   tmpl_dbi_t dbi,
   int key_id,
   char **value)
{
  int rc;
  MDB_val key, data;
  memset(&key, 0, sizeof(key));
  memset(&data, 0, sizeof(data));
  key.mv_size = sizeof(key_id);
  key.mv_data = &key_id;
  rc = mdb_get(txn, dbi, &key, &data);
  if (rc == 0) {
    *value = strndup((const char *) data.mv_data, data.mv_size);
  } else {
    *value = NULL;
  }
  return rc;
}

int storage_get_integer
  (tmpl_txn_t txn,
   tmpl_dbi_t dbi,
   int key_id,
   int *value)
{
  int rc;
  MDB_val key, data;
  memset(&key, 0, sizeof(key));
  memset(&data, 0, sizeof(data));
  key.mv_size = sizeof(key_id);
  key.mv_data = &key_id;
  rc = mdb_get(txn, dbi, &key, &data);
  if (rc == 0) {
    *value = *((int *) data.mv_data);
  } else {
    *value = 0;
  }
  return rc;
}

int storage_put_string(tmpl_txn_t txn, tmpl_dbi_t dbi, int key_id, const char *value, int length)
{
    int rc;
    MDB_val key, data;
    assert(txn);
    assert(dbi);
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));
    key.mv_size = sizeof(key_id);
    key.mv_data = &key_id;
    data.mv_size = strnlen(value, length);
    data.mv_data = strndup(value, length);
    rc = mdb_put(txn, dbi, &key, &data, 0);
    rc = 0; /* FIXME: delete */
    return rc;
}

int storage_create_directory(tmpl_dir_t **dir)
{
    tmpl_dir_t *p = calloc(sizeof(tmpl_dir_t), 1);
    if (p == NULL)
        return 1;
    *dir = p;
    return 0;
}

int storage_destroy_directory(tmpl_dir_t *dir)
{
    free(dir);
    return 0;
}

char *storage_dir_get_name(tmpl_dir_t *dir)
{
    assert(dir);
    return dir->name;
}

int storage_get_directory
  (tmpl_txn_t txn,
   tmpl_dbi_t dbi,
   int key_id,
   tmpl_dir_t *dir)
{
  FILE *file;
  int rc;
  int next_id;
  int name_length;
  char *name;
  MDB_val key, data;
  memset(&key, 0, sizeof(key));
  memset(&data, 0, sizeof(data));
  key.mv_size = sizeof(key_id);
  key.mv_data = &key_id;
  rc = mdb_get(txn, dbi, &key, &data);
  file = fmemopen(data.mv_data, data.mv_size, "rb");
  fread(&next_id, sizeof(next_id), 1, file);
  fread(&name_length, sizeof(name_length), 1, file);
  name = calloc(sizeof(*name), name_length + 1);
  fread(name, 1, name_length, file);
  fclose(file);
  memset(dir, 0, sizeof(*dir));
  dir->next_id = next_id;
  dir->name = name;
  return rc;
}

#endif /* USE_STORAGE */
