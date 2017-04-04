/* Copyright (C) 2017 Mateus de Lima Oliveira */

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <libintl.h>
#include <lmdb.h>
#include "config.h"

#define _ gettext

static const char copyright[] = COPYRIGHT;

int main(int argc, char **argv)
{
	int key_size = -1, value_size = -1;
	int c;
	char *path = ".";
	MDB_env *env;
	MDB_txn *txn;
	MDB_dbi dbi;
	MDB_val key, value;
	char *key_buffer, *value_buffer;
	char *msg;
	
	while ((c = getopt(argc, argv, "e:d:k:v:")) != -1) {
		switch (c) {
			case 'e':
			path = optarg;
			break;
			break;
			case 'k':
			key_size = atoi(optarg);
			if (key_size <= 0) {
				fprintf(stderr, "%s: %s\n", argv[0], _("key size must be positive"));
				return 1;
			}
			break;
			case 'v':
			value_size = atoi(optarg);
			if (value_size < 0) {
				fprintf(stderr, "%s: %s\n", argv[0], _("value size shall not be negative"));
				return 1;
			}
			break;
			case '?':
			return 1;
			default:
			abort();
		}
	}
	
	if (key_size <= 0) {
		fprintf(stderr, "%s: %s\n", argv[0], _("key size not specified"));
		return 1;
	}

	/*printf("key size: %d\nvalue size: %d\n", key_size, value_size);*/

	if (mdb_env_create(&env) != 0) {
		fprintf(stderr, "%s: %s\n", argv[0], _("could not create LMDB environment"));
		return 1;
	}
	mdb_env_set_maxdbs(env, 1);
	switch (c = mdb_env_open(env, path, 0, 0777)) {
		case 0:
		break;
		case MDB_VERSION_MISMATCH:
		msg = _("the version of the LMDB library doesn't match the version that created the database environment");
		goto err;
		case MDB_INVALID:
		msg = _("the environment file headers are corrupted");
		goto err;
		case ENOENT:
		case EACCES:
		case EAGAIN:
		msg = path;
		goto err2;
		default:
		msg = "mdb_env_open";
		goto err;
	}
	switch (c = mdb_txn_begin(env, NULL, 0, &txn)) {
		case 0:
		break;
		case MDB_PANIC:
		msg = _("a fatal error occurred earlier and the environment must be shut down");
		goto err;
		case MDB_MAP_RESIZED:
		msg = _("another process wrote data beyond this MDB_env's mapsize and this environment's map must be resized as well");
		goto err;
		case MDB_READERS_FULL:
		msg = _("a read-only transaction was requested and the reader lock table is full");
		goto err;
		case ENOMEM:
		msg = _("begin transaction");
		goto err2;
		default:
		msg = _("mdb_txn_begin: unknown error");
		goto err;
	}

	/* FIXME: check if the user has supplied the database name. */
	switch (mdb_dbi_open(txn, argv[optind], MDB_CREATE, &dbi)) {
		case 0:
		break;
		case MDB_NOTFOUND:
		msg = "the specified database doesn't exist in the environment and MDB_CREATE was not specified";
		goto err;
		case MDB_DBS_FULL:
		msg = "too many databases have been opened";
		goto err;
		default:
		msg = _("mdb_dbi_open: unknown error");
		goto err;
	}

	key_buffer = malloc(key_size);
	if (key_buffer == NULL)
		goto nomem;
	if (fread(key_buffer, key_size, 1, stdin) != 1)
		goto broken_pipe;
	key.mv_size = key_size;
	key.mv_data = key_buffer;

	if (value_size < 0) {
		switch (mdb_get(txn, dbi, &key, &value)) {
			case 0:
			break;
			case MDB_NOTFOUND:
			msg = _("the key was not in the database");
			goto err;
			default:
			msg = _("mdb_get: unknown error");
			goto err;
		}
		if (fwrite(value.mv_data, value.mv_size, 1, stdout) != 1)
			goto broken_pipe;
	} else {
		value_buffer = malloc(value_size);
		if (value_buffer == NULL)
			goto nomem;
		if (fread(value_buffer, value_size, 1, stdin) != 1)
			goto broken_pipe;

		value.mv_size = value_size;
		value.mv_data = value_buffer;
		switch (c = mdb_put(txn, dbi, &key, &value, 0)) {
			case 0:
			break;
			case MDB_MAP_FULL:
			msg = _("the database is full");
			goto err;
			case MDB_TXN_FULL:
			msg = _("the transaction has too many dirty pages");
			goto err;
			case EACCES:
			case EINVAL:
			msg = "mdb_put";
			goto err2;
			default:
			msg = _("mdb_put: unknown error");
			goto err;
		}
		free(value_buffer);
	}
	free(key_buffer);
	/* TODO: check return value. */
	mdb_txn_commit(txn);
	/* TODO: check return value. */
	mdb_dbi_close(env, dbi);
	mdb_env_close(env);
	return 0;

nomem:
	fprintf(stderr, "%s: %s\n", argv[0], strerror(ENOMEM));
	return 1;
broken_pipe:
	fprintf(stderr, "%s: broken pipe\n", argv[0]);
	return 1;
err:
	fprintf(stderr, "%s: %s\n", argv[0], msg);
	return 1;
err2:
	fprintf(stderr, "%s: %s: %s\n", argv[0], msg, strerror(c));
	return 1;
}
