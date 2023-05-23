import core.sys.posix.unistd;
import core.sys.posix.stdlib;
import core.sys.posix.stdio;
import core.sys.posix.sys.wait;

extern(C) {
    extern __gshared char **environ;
}

enum POLLEN_LIBRARY_SEARCH_PATH = "";

@safe
int WTERMSIG(int status)
{
    return ((status) & 0x7f);
}

@safe
int WIFEXITED(int status)
{
    return (WTERMSIG(status) == 0);
}

@safe
int WEXITSTATUS(int status)
{
    return (((status) & 0xff00) >> 8);
}

int link_file(string filename, string outputname)
{
    string pollenrt0 = "pollenrt0.o";
    assert(filename != null);
    assert(outputname != null);
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        const(char *)[9] args = ["cc",
            "-L", POLLEN_LIBRARY_SEARCH_PATH,
            pollenrt0.ptr, filename.ptr, "-lpollen",
            "-o", outputname.ptr, null];
        if (execve("/usr/bin/cc", args.ptr, environ) == -1) {
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

