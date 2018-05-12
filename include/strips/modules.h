#pragma once
#include <duktape.h>
#include <strips/definitions.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef duk_ret_t (*strips_module_resolve_cb)(duk_context *);
typedef duk_ret_t (*strips_module_load_cb)(duk_context *);

strips_ret_t duk_module_add_fn(duk_context *ctx, const char *name,
                               duk_c_function fn);

strips_ret_t duk_module_add_str(duk_context *ctx, const char *name,
                                const char *script);

strips_ret_t duk_module_add_lstr(duk_context *ctx, const char *name,
                                 const char *script, duk_size_t size);

duk_bool_t duk_module_has(duk_context *ctx, const char *name);

duk_ret_t strips_set_module_resolver(duk_context *ctx, const char *protocol,
                                     strips_module_resolve_cb resolve,
                                     strips_module_load_cb load);

#ifdef __cplusplus
}
#endif