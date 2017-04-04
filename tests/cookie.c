#include <stdio.h> /* printf() */
#include <stdlib.h> /* getenv() */
#include <templatizer.h>

static void cookiecb(void *user, char *key, int key_len, char *value, int value_len)
{
	printf("%.*s: %.*s\n", key_len, key, value_len, value);
}

int main()
{
	setenv("HTTP_COOKIE", "wi4c=1; _ga=GA1.1.2015232838.1486525612; token=c97da12b45f02b409affa48b3f3cb2cf97b125189b60e2c6a8c48fbc01c86b244a2c4b5dd1e07b7327bdd2e05cda6f31e0c4a85784ed1136f75dfe3ea192e63d", 1);
	parse_cookie_string(NULL, (kv_callback) cookiecb);
	return 0;
}
