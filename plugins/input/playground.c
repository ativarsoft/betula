/* Copyright (C) 2023 Mateus de Lima Oliveira */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>
#include <assert.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <pollen/pollen.h>
#include <pollen/query.h>
#include <pollen/stream.h>

#define FILE_EXTENSION ".tmpl"
#define BUFFER_SIZE 4096

typedef const char *string_t;

int run_file(string_t filename, FILE *file);

static thread_local tmpl_ctx_t playground_ctx = NULL;
static thread_local tmpl_cb_t playground_cb = NULL;

/*static const char mime[] = "application/x-www-form-urlencoded";*/

int save_data(string_t data, size_t len)
{
	int rc = -1;
	int fd = 0;
	size_t remaining = 0, bytes_written = 0;
	char filename[PATH_MAX] = {0};
	FILE *file = NULL;

	strcpy(filename, "playground-test-XXXXXX" FILE_EXTENSION);
	fd =  mkstemps(filename, sizeof(FILE_EXTENSION) - 1);
	if (fd < 0) {
		perror("mkstemps");
		return 1;
	}
	unlink(fd);
	file = fdopen(fd, "wt");
	if (file == NULL) {
		perror("fdopen");
		return 1;
	}
	remaining = len;
	while (remaining > 0) {
		bytes_written = fwrite(data, 1, remaining, file);
		remaining -= bytes_written;
	};
	fflush(file);
	rc = run_file(filename, file);
	if (rc != 0)
		return 1;
	fclose(file);
	return 0;
}

int query_callback
    (void *data,
     void *key, size_t keylen,
     void *value, size_t valuelen)
{
	char *decoded_ptr = NULL;
	size_t decoded_length = 0;
	if (strncmp(key, "script", keylen) == 0) {
		decoded_ptr = (char *) playground_cb->malloc(playground_ctx, BUFFER_SIZE);
		if (decoded_ptr == NULL) {
			perror("pollen malloc");
			return 1;
		}
		decoded_length = tmpl_percent_decoded_len(value, valuelen);
		tmpl_percent_decode_array(value, valuelen, decoded_ptr, decoded_length, &decoded_length);
		if (save_data(decoded_ptr, decoded_length) != 0)
			return 1;
	}
        return 0;
}

static int init(tmpl_ctx_t data, tmpl_cb_t cb)
{
	playground_ctx = data;
	playground_cb = cb;
	tmpl_fputs("Access-Control-Allow-Origin: *\r\n", stdout);
	fflush(stdout);
	char *method = getenv("REQUEST_METHOD");
        if (strcmp(method, "POST") == 0) {
                tmpl_parse_query_string_post(data, query_callback);
		exit(0);
        }
	return 0;
}

static void quit() {}

const tmpl_plugin_record_t templatizer_plugin_v1 = {
	&init,
	&quit
};

int run_file(string_t filename, FILE *file)
{
    assert(filename != NULL);
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        char *args[] = {"pollen", NULL};
        extern char **environ;
	setenv("PATH_TRANSLATED", filename, 1);
        if (execve("/usr/bin/pollen", args, environ) == -1) {
            perror("execve");
            return 1;
        }
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return 2;
        }
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Compilation failed\n");
            return 3;
        }
    }
    return 0;
}

