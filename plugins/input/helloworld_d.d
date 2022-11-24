/* Copyright (C) 2022 Mateus de Lima Oliveira */

import templatizer;

enum BUFFER_SIZE = 20;

extern(C)
static int init(tmpl_ctx_t data, tmpl_cb_t cb)
{
	import core.stdc.stdio : snprintf;
	import core.stdc.string : memset;
        import core.stdc.stdlib : calloc, free;

	char[BUFFER_SIZE] buffer = (cast(char *) calloc(char.sizeof, BUFFER_SIZE))[0..BUFFER_SIZE];
	SafeInt value;

	cb.send_default_headers(data);
	cb.add_filler_text(data, "Hello world from D!");

	memset(buffer.ptr, 0, BUFFER_SIZE);
	value = SafeInt.createSafeInt();
	value = value + 1 + 1;
	snprintf(buffer.ptr, BUFFER_SIZE, "%d", cast(int) value.get());
	cb.add_filler_text(data, buffer.ptr);
        free(cast(void *) value);

	memset(buffer.ptr, 0, BUFFER_SIZE);
        value = SafeInt.createSafeInt();
        value = 32;
        value = value + 8;
	snprintf(buffer.ptr, BUFFER_SIZE, "%d", cast(int) value.get());
	cb.add_filler_text(data, buffer.ptr);
        free(cast(void *) value);

        memset(buffer.ptr, 0, BUFFER_SIZE);
        value = SafeInt.createSafeInt();
        value = 5;
        value = value - 2;
	snprintf(buffer.ptr, BUFFER_SIZE, "%d", cast(int) value.get());
	cb.add_filler_text(data, buffer.ptr);
        free(cast(void *) value);

        memset(buffer.ptr, 0, BUFFER_SIZE);
        value = SafeInt.createSafeInt();
        value = 5;
        value = value * 2;
	snprintf(buffer.ptr, BUFFER_SIZE, "%d", cast(int) value.get());
	cb.add_filler_text(data, buffer.ptr);
        free(cast(void *) value);

	return 0;
}

extern(C)
static void quit() {}

extern(C)
templatizer_plugin templatizer_plugin_v1 = {
	&init,
	&quit
};
