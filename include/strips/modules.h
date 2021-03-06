#pragma once
#include <duktape.h>
#include <stdbool.h>
#include <strips/definitions.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef duk_ret_t (*strips_module_resolve_cb)(duk_context *);
typedef duk_ret_t (*strips_module_load_cb)(duk_context *);

strips_ret_t duk_module_add_fn_idx(duk_context *ctx, const char *name,
                                   duk_idx_t idx);

strips_ret_t duk_module_add_fn(duk_context *ctx, const char *name,
                               duk_c_function fn);

strips_ret_t duk_module_add_str(duk_context *ctx, const char *name,
                                const char *script);

strips_ret_t duk_module_add_lstr(duk_context *ctx, const char *name,
                                 const char *script, duk_size_t size);

duk_bool_t duk_module_push(duk_context *ctx, const char *name);

duk_bool_t duk_module_has(duk_context *ctx, const char *name);

duk_ret_t strips_set_module_resolver(duk_context *ctx, const char *protocol,
                                     strips_module_resolve_cb resolve,
                                     strips_module_load_cb load);

duk_ret_t strips_unset_module_resolver(duk_context *ctx, const char *protocol);

bool strips_set_module_parser(duk_context *ctx, const char *ext,
                              duk_c_function fn);

bool strips_get_module_parser(duk_context *ctx, const char *ext);

#ifdef __cplusplus
}
#endif