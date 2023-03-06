/* Copyright (C) 2022 Mateus de Lima Oliveira */

#ifndef TMPL_SQL_H
#define TMPL_SQL_H

/*
 * This file contains an API for SQL databases that can easily callable
 * from safe languages.
 */

/*
 * API calling convention:
 *   * each function return a status code on a register
 *   * handles are returned as explicit pointers
 */

/* currently selected sql plugin handle */
extern void *sql_plugin;

typedef sqlite3 tmpl_sql_t;

int sql_initialize();
int sql_connect(tmpl_sql_t *connection, const char *parms);
int sql_disconnect(tmpl_sql_t *connection);
int sql_execute(tmpl_sql_t *connection);
int sql_prepare();
int sql_finalize();

#endif
