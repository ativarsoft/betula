#include <templatizer.h>
#include <unistd.h>
#include <stddef.h>

int init(tmpl_ctx_t data, struct templatizer_callbacks *cb)
{
    const char *executable = NULL;
    size_t length = 0;
    int rc = -1;
    int num_parameters = 0;
    char * const* argv = NULL;
    char * const* envp = {NULL}; /* No environment variables. */
    int i;
    /* Get the first plugin parameter.
     * The first plugin parameter is the executable file path. */
    num_parameters = cb->get_num_plugin_parameters(data);
    rc = cb->get_plugin_parameter(data, 0, &executable, &length);
    if (rc)
	return rc;
    argv = cb->malloc(data, sizeof(*argv) * num_parameters + 1);
    for (i = 0; i < num_parameters; i++) {
        /* TODO: build parameter list. */
    }
    execve(executable, argv, envp);
    return 0;
}

void quit() {}

struct templatizer_plugin templatizer_plugin_v1 = {
    &init,
    &quit
};
