/* Copyright (C) 2017 Mateus de Lima Oliveira */

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <templatizer/json.h>
#include <laxjson.h>

struct object_item {
	struct object_item *next;
	struct tmpl_json_property *object;
};

struct tmpl_json_context {
	struct LaxJsonContext *parserctx;
	struct object_item *objstack;
};

static int on_string(struct LaxJsonContext *parserctx, enum LaxJsonType type,
	const char *value, int length)
{
	struct tmpl_json_context *context = (struct tmpl_json_context *) parserctx->userdata;
	if (type == LaxJsonTypeProperty) {
		for (obj = &context->objstack->item.data; obj; obj++) {
			
		}
	} else {
		/*if (property == _access_token)
			access_token = strndup(value, length);
		property = 0;*/
	}
	return 0;
}

static int on_number(struct LaxJsonContext *parserctx, double x)
{
	struct tmpl_json_context *context;

	context = (struct tmpl_json_context *) parserctx->userdata;
	context->objstack->item.data.number = x;
	return 0;
}

static int on_primitive(struct LaxJsonContext *context, enum LaxJsonType type)
{
	switch (type) {
		case LaxJsonTypeTrue:
		break;
		case LaxJsonTypeFalse:
		break;
		default:
		break;
	}
	return 0;
}

static int on_begin(struct LaxJsonContext *parserctx, enum LaxJsonType type)
{
	struct object_item *obj;
	struct tmpl_json_context *context;

	context = (struct tmpl_json_context *) parserctx->userdata;
	if ((obj = malloc(sizeof(struct object_item))) == NULL)
		return 1;
	obj->next = context->objstack;
	context->objstack = obj;
	return 0;
}

static int on_end(struct LaxJsonContext *parserctx, enum LaxJsonType type)
{
	struct object_item *obj;
	struct tmpl_json_context *context;

	context = (struct tmpl_json_context *) parserctx->userdata;
	obj = context->objstack;
	context->objstack = obj->next;
	free(obj);
	return 0;
}

int tmpl_json_feed(struct tmpl_json_context *context, size_t size, void *data)
{
	 return lax_json_feed(context->parserctx, size, data);
}

int tmpl_json_eof(struct tmpl_json_context *context)
{
	return lax_json_eof(context->parserctx);
}

struct tmpl_json_context *tmpl_json_create(void)
{
	struct LaxJsonContext *parserctx;
	struct tmpl_json_context *context;

	if ((parserctx = lax_json_create()) == NULL)
		return NULL;
	context = malloc(sizeof(struct tmpl_json_context));
	if (context == NULL) {
		lax_json_destroy(parserctx);
		return NULL;
	}
	parserctx->userdata = NULL;
	parserctx->string = &on_string;
	parserctx->number = &on_number;
	parserctx->primitive = &on_primitive;
	parserctx->begin = &on_begin;
	parserctx->end = &on_end;
	context->parserctx = parserctx;
	context->objstack = NULL;
	return context;
}

void tmpl_json_destroy(struct tmpl_json_context *context)
{
	lax_json_destroy(context->parserctx);
}
