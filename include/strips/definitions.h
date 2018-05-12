#pragma once

#ifdef __cplusplus
extern "C" {
#endif
typedef enum strips_log_level_s { STRIPS_DEBUG, STRIPS_INFO, STRIPS_WARN, STRIPS_ERROR } strips_log_level_t;


typedef struct strips_bag_t {
  duk_context *ctx;
  int ref; // Reference in the ref store to the callback
  void *data;
  duk_size_t size;
  int flag;
} strips_bag_t;

typedef enum strips_ret_t {
  STRIPS_OK,
  STRIPS_DUPLICATE_MODULE,
  STRIPS_ALREADY_INITIALIZED,
  STRIPS_NOT_INITIALIZED,

  STRIPS_MOUDLE_NOT_FOUND

} strips_ret_t;


typedef duk_ret_t (*strips_module_initializer)(duk_context *ctx);

#ifdef __cplusplus
}
#endif