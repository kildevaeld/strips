#pragma once

typedef enum strips_ret_t {
  STRIPS_OK,
  STRIPS_DUPLICATE_MODULE,
  STRIPS_ALREADY_INITIALIZED,
  STRIPS_NOT_INITIALIZED,

  STRIPS_MOUDLE_NOT_FOUND

} strips_ret_t;

typedef duk_ret_t (*strips_module_initializer)(duk_context *ctx);
