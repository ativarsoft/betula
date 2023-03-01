#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/wait.h>
#include "linker.h"
#include "config.h"

int link_file(string_t filename, string_t outputname)
{
    const char *pollenrt0 =
#ifdef POLLENRT0_PATH
        POLLENRT0_PATH;
#else
        "pollenrt0.o";
#endif
    assert(filename != NULL);
    assert(outputname != NULL);
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        const char *args[] = {"cc",
            "-L", POLLEN_LIBRARY_SEARCH_PATH,
            pollenrt0, filename, "-lpollen",
            "-o", outputname, NULL};
        extern char **environ;
        if (execve("/usr/bin/cc", args, environ) == -1) {
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

