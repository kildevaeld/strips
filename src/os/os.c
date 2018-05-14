#include "../src/private.h"
#include <csystem/string.h>
#include <strips/modules.h>
#include <strips/os/os.h>
#include <strips/utils.h>

extern char **environ;

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
  setenv(n, v, 1);

  return 0;
}

static duk_ret_t duk_os_env_has(duk_context *ctx) {
  duk_push_boolean(ctx, getenv(duk_require_string(ctx, 1)));
  return 1;
}

static duk_ret_t duk_os_env_del(duk_context *ctx) { return 0; }

static duk_ret_t duk_os_env_ownkeys(duk_context *ctx) {

  duk_push_array(ctx);
  int i = 0;

  while (environ[i]) {
    int idx = cs_str_indexof(environ[i], '=');
    duk_push_lstring(ctx, environ[i], idx);
    duk_put_prop_index(ctx, -2, i);
    // see https://github.com/svaarala/duktape/issues/1543
    duk_push_boolean(ctx, true);
    duk_put_prop_lstring(ctx, 0, environ[i++], idx);
  }

  return 1;
}

static duk_ret_t duk_os_module(duk_context *ctx) {
  duk_push_object(ctx);

  strips_get_entry(ctx, "os");
  duk_get_prop_string(ctx, -1, "argv");
  duk_put_prop_string(ctx, -3, "argv");
  duk_get_prop_string(ctx, -1, "env");
  duk_put_prop_string(ctx, -3, "env");
  duk_pop(ctx);

  return 1;
}

void strips_os_init(duk_context *ctx, int argc, char *argv[], char **env) {
  if (strips_has_entry(ctx, "os")) {
    return;
  }
  duk_push_object(ctx);
  if (argv != NULL) {
    duk_push_array(ctx);
    for (int i = 0; i < argc; i++) {
      duk_push_string(ctx, argv[i]);
      duk_put_prop_index(ctx, -2, i);
    }
  } else {
    duk_push_undefined(ctx);
  }
  duk_put_prop_string(ctx, -2, "argv");

  duk_push_object(ctx);

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

  duk_put_prop_string(ctx, -2, "env");

  strips_push_entry(ctx, "os");

  duk_module_add_fn(ctx, "os", duk_os_module);
}