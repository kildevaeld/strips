#include "../src/private.h"
#include <errno.h>
#include <strips/modules.h>
#include <strips/os/os.h>
#include <strips/utils.h>

extern duk_ret_t duk_io_readdir(duk_context *ctx);
extern duk_ret_t duk_io_mkdir(duk_context *ctx);

extern duk_ret_t duk_io_getcwd(duk_context *ctx);

extern void strips_io_push_env(duk_context *ctx);

static duk_ret_t duk_os_module(duk_context *ctx) {
  duk_push_object(ctx);

  strips_get_entry(ctx, "os");
  duk_get_prop_string(ctx, -1, "argv");
  duk_put_prop_string(ctx, -3, "argv");
  duk_pop(ctx);

  duk_push_object(ctx);

  strips_io_push_env(ctx);
  duk_put_prop_string(ctx, -2, "env");

  duk_push_c_function(ctx, duk_io_readdir, 2);
  duk_put_prop_string(ctx, -2, "readdir");

  duk_push_c_function(ctx, duk_io_mkdir, 2);
  duk_set_magic(ctx, -1, 0);
  duk_put_prop_string(ctx, -2, "mkdir");

  duk_push_c_function(ctx, duk_io_mkdir, 2);
  duk_set_magic(ctx, -1, 1);
  duk_put_prop_string(ctx, -2, "mkdirAll");

  duk_push_c_function(ctx, duk_io_getcwd, 0);
  duk_put_prop_string(ctx, -2, "getwd");

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

  strips_push_entry(ctx, "os");

  duk_module_add_fn(ctx, "os", duk_os_module);
}