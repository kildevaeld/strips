#pragma once
#include <duktape.h>
#include <strips/definitions.h>

strips_ret_t duk_module_add_fn(duk_context *ctx, const char *name,
                               duk_c_function fn);

strips_ret_t duk_module_add_str(duk_context *ctx, const char *name,
                                const char *script);

strips_ret_t duk_module_add_lstr(duk_context *ctx, const char *name,
                                 const char *script, duk_size_t size);

duk_bool_t duk_module_has(duk_context *ctx, const char *name);