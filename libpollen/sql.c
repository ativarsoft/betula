/* Copyright (C) 2022 Mateus de Lima Oliveira */

#include <sqlite3.h>
#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <pollen/sql.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	for(i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

int sql_initialize()
{
	return 0;
}

int sql_connect(tmpl_sql_t *connection, const char *parms)
{
	sqlite3 *db;
	int rc;
	assert(connection == NULL);
	rc = sqlite3_open(parms, &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		connection = NULL;
		return 1;
	}
	connection = db;
	return 0;
}

int sql_disconnect(tmpl_sql_t *connection)
{
	sqlite3_close(connection);
	return 0;
}

int sql_execute(tmpl_sql_t *connection)
{
	int rc;
	char *zErrMsg = 0;
	sqlite3 *db = connection;
	rc = sqlite3_exec(db, "", callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	return 0;
}

int sql_prepare()
{
	return 0;
}

int sql_finalize()
{
	return 0;
}
