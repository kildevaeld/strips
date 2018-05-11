#pragma once
#include <duktape.h>
#ifdef __cplusplus
extern "C" {
#endif

duk_bool_t duk_io_is_writerlike(duk_context *ctx, duk_idx_t idx);
duk_bool_t duk_io_is_writer(duk_context *ctx, duk_idx_t idx);
duk_bool_t duk_io_is_reader(duk_context *ctx, duk_idx_t idx);
duk_bool_t duk_io_is_file(duk_context *ctx, duk_idx_t idx);

void strips_io_init(duk_context *ctx);

#ifdef __cplusplus
}
#endif