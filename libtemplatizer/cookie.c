#include <templatizer/cookie.h>
#include <stdio.h>
#include <string.h> /* strchr() */
#include <stdlib.h> /* getenv() */

/* based on rfc6265 */
/*
   cookie-header = "Cookie:" OWS cookie-string OWS
   cookie-string = cookie-pair *( ";" SP cookie-pair )
   cookie-pair   = cookie-name "=" cookie-value
*/
int tmpl_parse_cookie_string(void *data, http_cookie_callback_t cb)
{
	char *query, *p, *token;
	char *key, *value;
	size_t key_len, value_len;

	query = getenv("HTTP_COOKIE");
	if (query == NULL)
		return 1;
	p = query;
	for (;;) {
		/* get key's length */
		token = strchr(p, '=');
		if (token == NULL)
			break; /* No key after amp or key has no value. */
		key = p;
		key_len = token - p;
		p += key_len + 1;

		/* get value's length */
		token = strchr(p, ';');
		if (token) {
			/* This is NOT the last value. */
			value = p;
			value_len = token - p;
			p += value_len + 1;
			if (*(p++) != ' ') {
				fputs("missing space after ';' on cookie string\n", stderr);
				return 1;
			}
			cb(data, key, key_len, value, value_len);
		} else {
			/* This is the last value. */
			value_len = strlen(p);
			value = p;
			cb(data, key, key_len, value, value_len);
			p += value_len + 1;
			break;
		}
	}
	return 0;
}
