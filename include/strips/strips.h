#pragma once
#include <duktape.h>
#include <strips/definitions.h>

#ifdef __cplusplus
extern "C" {
#endif

strips_ret_t strips_initialize(duk_context *ctx);

duk_ret_t strips_eval_path(duk_context *ctx, const char *path, char **err);

duk_ret_t strips_eval_script(duk_context *ctx, const char *script, const char *path, char **err);

#ifdef __cplusplus
}
#endif