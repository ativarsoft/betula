#include <templatizer.h>
#include <stdio.h>

static char *get_string_constant_label(tmpl_ctx_t ctx, const char *el)
{
    return "strffff1234"; /* str + addr_in_the_tag_pool */
}

static int on_text_tag_start(tmpl_ctx_t ctx, const char *el, const char **attr)
{
    char *el_label = get_string_constant_label(ctx, el);
    //tmpl_string_t attr_const = cb->get_string_constant(ctx, el);
    printf("push %s\n", el_label);
    //printf("push %s\n", attr_const->attr);
    printf("call tmpl_print_tag_start\n");
    return 0;
}

static int on_text_tag_end(tmpl_ctx_t ctx, const char *el)
{
    printf("call tmpl_print_tag_end");
    return 0;
}

static int init(tmpl_ctx_t ctx, struct templatizer_callbacks *cb)
{
    cb->register_codegen_tag_start
        (ctx, &on_text_tag_start);
    cb->register_codegen_tag_end
        (ctx, &on_text_tag_end);
    return 0;
}

static void quit() {}

struct templatizer_plugin templatizer_plugin_v1 = {
    &init,
    &quit
};
