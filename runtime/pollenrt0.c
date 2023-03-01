#include "config.h"

#ifdef SANITY_TEST
#include <stdio.h>
#endif

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#ifdef SANITY_TEST
int pollen_main(int x, int y);
#else
int pollen_main(void);
#endif

/*
 * Global variables
 */


/* argc and argv are saved to be used during the
 * program execution by calling the runtime functions
 * that manage them. */
EXPORT int pollen_argc = 0;
EXPORT char **pollen_argv = {0};

/*
 * The main function called by 'crt0'.
 */

int main(int argc, char *argv[])
{
#ifndef SANITY_TEST
	pollen_argc = argc;
	pollen_argv = argv;
	return pollen_main();
#else
	int result = 0;
        int x = 0, y = 0;
	fputs("Type a number: ", stdout);
	scanf("%d", &x);
	fputs("Type a number: ", stdout);
	scanf("%d", &y);
	result = pollen_main(x, y);
        printf("%d\n", result);
	return 0;
#endif
}
