#include <duktape.h>
#include <errno.h>
#include <stdbool.h>

extern char **environ;

static size_t indexof(const char *path, char c) {
  size_t len = strlen(path);
  for (size_t i = 0; i < len; i++) {
    if (path[i] == c) return i;
  }

  return -1;
}

static duk_ret_t duk_os_env_get(duk_context *ctx) {
  char *e = getenv(duk_require_string(ctx, 1));
  if (!e) {
    duk_push_null(ctx);
  } else {
    duk_push_string(ctx, e);
  }
  return 1;
}

static duk_ret_t duk_os_env_set(duk_context *ctx) {
  const char *n = duk_require_string(ctx, 1);
  const char *v = duk_require_string(ctx, 2);
  if (setenv(n, v, 1) != 0) {
    duk_type_error(ctx, "%s", strerror(errno));
  }

  return 0;
}

static duk_ret_t duk_os_env_has(duk_context *ctx) {
  duk_push_boolean(ctx, getenv(duk_require_string(ctx, 1)));
  return 1;
}

static duk_ret_t duk_os_env_del(duk_context *ctx) {
  const char *k = duk_require_string(ctx, 1);
  duk_del_prop_string(ctx, 0, k);
  if (unsetenv(k) != 0) {
    duk_type_error(ctx, "%s", strerror(errno));
  }

  return 0;
}

static duk_ret_t duk_os_env_ownkeys(duk_context *ctx) {

  duk_push_array(ctx);
  int i = 0;

  while (environ[i]) {
    int idx = indexof(environ[i], '=');
    duk_push_lstring(ctx, environ[i], idx);
    duk_put_prop_index(ctx, -2, i);
    // see https://github.com/svaarala/duktape/issues/1543
    duk_push_boolean(ctx, true);
    duk_put_prop_lstring(ctx, 0, environ[i++], idx);
  }

  return 1;
}

void strips_io_push_env(duk_context *ctx) {
  duk_push_object(ctx);
  duk_push_c_function(ctx, duk_os_env_get, 2);
  duk_put_prop_string(ctx, -2, "get");
  duk_push_c_function(ctx, duk_os_env_set, 3);
  duk_put_prop_string(ctx, -2, "set");
  duk_push_c_function(ctx, duk_os_env_has, 2);
  duk_put_prop_string(ctx, -2, "has");
  duk_push_c_function(ctx, duk_os_env_ownkeys, 1);
  duk_put_prop_string(ctx, -2, "ownKeys");
  duk_push_proxy(ctx, 0);
}