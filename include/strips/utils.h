#pragma once
#include <duktape.h>

void duk_dump_context_stdout(duk_context *ctx);

// like luaL_ref, but assumes storage in "refs" property of heap stash
int duk_ref(duk_context *ctx);
void duk_push_ref(duk_context *ctx, int ref);
void duk_unref(duk_context *ctx, int ref);

void duk_commonjs_wrapl(duk_context *ctx, const char *buffer, size_t len);

const char *duk_get_main(duk_context *ctx);