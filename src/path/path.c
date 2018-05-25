#include <strips/path/path.h>
#include <syrup/path.h>

static duk_ret_t duk_path_join(duk_context *ctx) {
  int len = duk_get_top(ctx);
  const char *paths[len + 1];
  int l = 0;
  for (int i = 0; i < len; i++) {
    paths[i] = duk_require_string(ctx, i);
    l += strlen(paths[i]) + 2;
  }
  paths[len] = NULL;

  // char buffer[l + 1];
  char *buffer = sy_path_join_array(NULL, paths);
  if (!buffer)
    return 0;

  duk_push_string(ctx, buffer);

  return 1;
}

static duk_ret_t duk_path_base(duk_context *ctx) {
  const char *path = duk_require_string(ctx, 0);

  size_t len = 0, idx = 0;
  if (!(len = sy_path_base(path, &idx))) {
    return 0;
  }

  duk_push_lstring(ctx, path + idx, len);

  return 1;
}

static duk_ret_t duk_path_dir(duk_context *ctx) {
  const char *path = duk_require_string(ctx, 0);

  int len = 0;
  if (!(len = sy_path_dir(path))) {
    return 0;
  }

  duk_push_lstring(ctx, path, len);

  return 1;
}

static duk_ret_t duk_path_ext(duk_context *ctx) { return 0; }

static duk_ret_t duk_path_resolve(duk_context *ctx) {

  const char *path = duk_require_string(ctx, 0);
  char *buffer = sy_path_resolve(path, NULL);
  duk_push_string(ctx, buffer);
  free(buffer);
  return 1;
}

static duk_ret_t strips_path_module(duk_context *ctx) {
  duk_push_object(ctx);

  duk_push_c_function(ctx, duk_path_join, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "join");

  duk_push_c_function(ctx, duk_path_base, 1);
  duk_put_prop_string(ctx, -2, "basename");

  duk_push_c_function(ctx, duk_path_dir, 1);
  duk_put_prop_string(ctx, -2, "dirname");

  duk_push_c_function(ctx, duk_path_ext, 1);
  duk_put_prop_string(ctx, -2, "extname");

  duk_push_c_function(ctx, duk_path_resolve, 2);
  duk_put_prop_string(ctx, -2, "resolve");

  return 1;
}

void strips_path_init(duk_context *ctx) {
  duk_module_add_fn(ctx, "path", strips_path_module);
}