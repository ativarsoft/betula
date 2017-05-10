/* Copyright (C) 2017 Mateus de Lima Oliveira */

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <libintl.h>

#define _ gettext

static const char copyright[] = "Copyright (C) 2017 Mateus de Lima Oliveira";

int main(int argc, char **argv)
{
	int key_size = -1, value_size = -1;
	int c;
	char *url;
	
	while ((c = getopt(argc, argv, "e:d:k:v:")) != -1) {
		switch (c) {
			case 'u':
			url = optarg;
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
	return 0;
}
