#include <stdio.h>
#include <stdlib.h>
#include <templatizer/percent.h>

static const char encoded[] = "a%40a";

int main()
{
	size_t len;
	char *decoded;

	len = percent_decoded_len(encoded);
	decoded = malloc(len + 1);
	if (decoded == NULL) {
		puts("no mem");
		return 1;
	}
	decoded[len] = '\0';
	percent_decode(encoded, decoded);
	printf("encoded: %s\ndecoded: %s (len = %lu)\n", encoded, decoded, len);
	return 0;
}
