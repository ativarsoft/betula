/* Copyright (C) 2017 Mateus de Lima Oliveira */

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <libintl.h>
#include <curl/curl.h>

#define _ gettext

static const char copyright[] = "Copyright (C) 2017 Mateus de Lima Oliveira";

char *url = NULL;
int post = 0;

int do_post()
{
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if (!curl)
		return 1;
	curl_easy_setopt(curl, CURLOPT_URL, url);
	if (post) {
		/* Use read callback function instead of string. */
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, NULL);
		/* Read from FILE * pointed by CURLOPT_READDATA. */
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		/* Leave stdin as file descriptor */
		curl_easy_setopt(curl, CURLOPT_READDATA, NULL);
	}
	res = curl_easy_perform(curl);
	if(res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
		curl_easy_strerror(res));
	curl_global_cleanup();
	return 0;
}

int main(int argc, char **argv)
{
	int key_size = -1, value_size = -1;
	int c;
	
	while ((c = getopt(argc, argv, "pu:d:k:v:")) != -1) {
		switch (c) {
			case 'p':
			post = 1;
			break;
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
	
	if (!url) {
		fprintf(stderr, "url not specified.\n");
		return 1;
	}

	do_post();

	return 0;
}
