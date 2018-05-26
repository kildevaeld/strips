#pragma once
#include <duktape.h>

void strips_commonjs_init(duk_context *ctx);
duk_ret_t strips_commonjs_eval_main(duk_context *ctx, const char *path);