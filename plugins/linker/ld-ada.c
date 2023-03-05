/* Copyright (C) 2023 Mateus de Lima Oliveira */

#include <pollen/pollen.h>
#include <threads.h>
#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>

void adainit();
void adafinal();
void Davinit();
void initialize_dav();
void finalize_dav();

thread_local tmpl_ctx_t dav_ctx;
thread_local tmpl_cb_t dav_cb;

void *handle;

void dav_send_default_headers()
{
    assert(dav_ctx && dav_cb);
    dav_cb->send_default_headers(dav_ctx);
}

void dav_filler_text(const char *s)
{
    assert(dav_ctx && dav_cb);
    dav_cb->add_filler_text(dav_ctx, s);
}

void dav_if(int a)
{
    assert(dav_ctx && dav_cb);
    dav_cb->add_control_flow(dav_ctx, a? IF_TRUE : IF_FALSE);
}

void dav_swhile(int a)
{
    assert(dav_ctx && dav_cb);
    dav_cb->add_control_flow(dav_ctx, a? SWHILE_TRUE : SWHILE_FALSE);
}

static int init(tmpl_ctx_t data, tmpl_cb_t cb)
{
    char *error;
    void (*tmpl_ada_initialize)();

    handle = dlopen("ada-plugin.so", RTLD_LAZY);
    if (handle == NULL) {
        fprintf(stderr, "ada: failed do open plugin: %s\n", dlerror());
        return 1;
    }
    dlerror();    /* Clear any existing error */
    tmpl_ada_initialize = (void (*)()) dlsym(handle, "tmpl_ada_initialize");
    if ((error = dlerror())) {
        fprintf(stderr, "ada: failed to load initialization symbol: %s\n", error);
        return 1;
    }
    adainit();
    dav_ctx = data;
    dav_cb = cb;
    tmpl_ada_initialize();
    return 0;
}

static void quit()
{
    char *error;
    void (*tmpl_ada_finalize)();

    tmpl_ada_finalize = (void (*)()) dlsym(handle, "tmpl_ada_finalize");
    if ((error = dlerror())) {
        fprintf(stderr, "ada: failed to load finalization symbol: %s\n", error);
    }
    tmpl_ada_finalize();
    adafinal();
    dlclose(handle);
}

const tmpl_plugin_record_t templatizer_plugin_v1 = {
    &init,
    &quit
};
