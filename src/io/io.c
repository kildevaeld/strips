#include "../script-data.h"
#include <strips/io/io.h>
#include <strips/modules.h>
#include <strips/types.h>
#include <unistd.h>

extern void duk_io_push_writer(duk_context *ctx);
extern void duk_io_push_reader(duk_context *ctx);
extern void duk_io_push_file(duk_context *ctx);

duk_bool_t duk_io_is_writerlike(duk_context *ctx, duk_idx_t idx) {
  if (duk_io_is_writer(ctx, idx)) {
    return true;
  }

  if (duk_has_prop_string(ctx, -1, "write")) {
    duk_get_prop_string(ctx, -1, "write");
    duk_bool_t ret = duk_is_function(ctx, -1);
    duk_pop(ctx);
    return ret;
  }
  return false;
}

duk_bool_t duk_io_is_readerlike(duk_context *ctx, duk_idx_t idx) {
  if (duk_io_is_reader(ctx, idx)) {
    return true;
  }

  if (duk_has_prop_string(ctx, -1, "read")) {
    duk_get_prop_string(ctx, -1, "read");
    duk_bool_t ret = duk_is_function(ctx, -1);
    duk_pop(ctx);
    return ret;
  }
  return false;
}

duk_bool_t duk_io_is_writer(duk_context *ctx, duk_idx_t idx) {
  duk_dup(ctx, idx);
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "Writer");

  duk_bool_t ret = 1;
  if (!duk_instanceof(ctx, -3, -1)) {
    ret = 0;
  }
  duk_pop_3(ctx);
  return ret;
}

duk_bool_t duk_io_is_reader(duk_context *ctx, duk_idx_t idx) {

  duk_dup(ctx, idx);
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "Reader");

  duk_bool_t ret = 1;
  if (!duk_instanceof(ctx, -3, -1)) {
    ret = 0;
  }
  duk_pop_3(ctx);

  return ret;
}

duk_bool_t duk_io_is_file(duk_context *ctx, duk_idx_t idx) {}

static duk_ret_t strips_io_module_init(duk_context *ctx) {

  duk_push_object(ctx);

  duk_io_push_writer(ctx);

  // We store on stash, for easy access
  duk_push_global_stash(ctx);
  duk_dup(ctx, -2);
  duk_put_prop_string(ctx, -2, "Writer");
  duk_pop(ctx);

  duk_put_prop_string(ctx, -2, "Writer");

  duk_io_push_reader(ctx);

  // We store on stash, for easy access
  duk_push_global_stash(ctx);
  duk_dup(ctx, -2);
  duk_put_prop_string(ctx, -2, "Reader");
  duk_pop(ctx);

  duk_put_prop_string(ctx, -2, "Reader");

  duk_io_push_file(ctx);

  duk_dup(ctx, -1);
  duk_global_type_register(ctx, "File");

  duk_put_prop_string(ctx, -2, "File");

  duk_push_int(ctx, SEEK_SET);
  duk_put_prop_string(ctx, -2, "SEEK_SET");

  duk_push_int(ctx, SEEK_CUR);
  duk_put_prop_string(ctx, -2, "SEEK_CUR");

  duk_push_int(ctx, SEEK_END);
  duk_put_prop_string(ctx, -2, "SEEK_END");

  duk_get_prop_string(ctx, -1, "File");
  duk_push_int(ctx, STDOUT_FILENO);
  duk_new(ctx, 1);
  duk_put_prop_string(ctx, -2, "stdout");

  duk_get_prop_string(ctx, -1, "File");
  duk_push_int(ctx, STDERR_FILENO);
  duk_new(ctx, 1);
  duk_put_prop_string(ctx, -2, "stderr");

  duk_get_prop_string(ctx, -1, "File");
  duk_push_int(ctx, STDIN_FILENO);
  duk_new(ctx, 1);
  duk_put_prop_string(ctx, -2, "stdin");

  duk_commonjs_wrapl(ctx, (const char *)io_js, io_js_len);
  duk_dup(ctx, -2);
  duk_get_global_string(ctx, "require");

  duk_ret_t ret = duk_pcall(ctx, 2);

  if (ret != DUK_EXEC_SUCCESS) {
    duk_throw(ctx);
  }

  duk_pop(ctx);

  return 1;
}

void strips_io_init(duk_context *ctx) {
  duk_module_add_fn(ctx, "io", strips_io_module_init);
}