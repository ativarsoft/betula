/* Copyright (C) 2017-2022 Mateus de Lima Oliveira */

#include <unistd.h>
#include <libgen.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h> /* atoi() */
#include <string.h>
#include <ctype.h>
#include <sys/queue.h>
#include <expat.h>
#include <templatizer.h>
#include <ctype.h>
#include <apr-1.0/apr_pools.h>
#include <apr-1.0/apr_strings.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <templatizer/compiler/compiler.h>
#include "memory.h"
#include "opcode.h"

static int serialize_node(tmpl_ctx_t ctx, struct node *n, FILE *file);

static int serialize_node(tmpl_ctx_t ctx, struct node *n, FILE *file)
{
	uint8_t type = {n->type};
	fwrite(&type, sizeof(type), 1, file);
	return 0;
}

static int serialize_all_nodes(tmpl_ctx_t ctx, FILE *file)
{
	struct node *n = NULL;
	int result = -1;
	int ret = -1;

	TAILQ_FOREACH(n, ctx->nodes, entries) {
		result = serialize_node(ctx, n, file);
		if (result != 0) {
			ret = 1;
			goto out1;
		}
	}
	ret = 0;
out1:
	return ret;
}

int serialize_template_file(tmpl_ctx_t ctx)
{
	int fd = -1;
	int ret = -1;
	int result = -1;
	FILE *file;
	char *ptr;
	size_t sizeloc;
	file = open_memstream(&ptr, &sizeloc);
	if (file == NULL) {
		goto error1;
	}
	result = serialize_all_nodes(ctx, file);
	if (result != 0) {
		goto error2;
	}
	fclose(file);
	ret = 0;
	goto ok;
error2:
	close(fd);
error1:
ok:
	return ret;
}
