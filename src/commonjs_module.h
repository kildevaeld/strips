#pragma once
#include <duktape.h>

duk_ret_t cjs_resolve_module(duk_context *ctx);
duk_ret_t cjs_load_module(duk_context *ctx);