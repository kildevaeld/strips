#pragma once
#include <duktape.h>

#ifdef __cplusplus
extern "C" {
#endif

void duk_dump_context_stdout(duk_context *ctx);

// like luaL_ref, but assumes storage in "refs" property of heap stash
int duk_ref(duk_context *ctx);
void duk_push_ref(duk_context *ctx, int ref);
void duk_unref(duk_context *ctx, int ref);

void duk_commonjs_wrapl(duk_context *ctx, const char *buffer, size_t len);

duk_bool_t duk_is_date(duk_context *ctx, duk_idx_t);

const char *duk_get_main(duk_context *ctx);

#ifdef __cplusplus
}
#endif