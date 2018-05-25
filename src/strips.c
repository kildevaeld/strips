#include "commonjs.h"
#include "commonjs_file.h"
#include "commonjs_module.h"
#include "console.h"
#include "file-utils.h"
#include "private.h"
#include <strips/modules.h>
#include <strips/strips.h>
#include <strips/utils.h>
#include <syrup/path.h>

//#include "path/path.h"
//#include "prompt/prompt.h"
#include "script-data.h"

static duk_ret_t get_module_resolver(duk_context *ctx) {
  // duk_push_global_stash(ctx);
  // duk_get_prop_string(ctx, -1, "resolvers");

  strips_get_entry(ctx, "resolvers");

  duk_get_prop_string(ctx, -1, duk_require_string(ctx, 0));

  return 1;
}

static void strips_initialize_stash(duk_context *ctx) {
  duk_push_object(ctx);

  // Setup refs
  duk_push_array(ctx);
  duk_push_int(ctx, 0);
  duk_put_prop_index(ctx, -2, 0);
  duk_put_prop_string(ctx, -2, "refs");
  // Setup Constants
  duk_eval_string(ctx,
                  "({"
                  "protocol: "
                  "/^([a-zA-Z0-9]+)(?:\\:\\/\\/)(\\/?[a-zA-Z0-9\\.\\-]+(?:\\/"
                  "[a-zA-Z0-9\\.\\-]+)*)$/,"
                  "file: /^(?:\\/|\\.\\.?\\/)(?:[^\\/\\0]+(?:\\/)?)+$/"
                  "})");
  duk_put_prop_string(ctx, -2, "constants");
  // Setup modules
  duk_push_object(ctx);
  duk_put_prop_string(ctx, -2, "modules");

  duk_push_object(ctx);
  duk_put_prop_string(ctx, -2, "resolvers");
  duk_push_c_lightfunc(ctx, get_module_resolver, 1, 1, 0);
  duk_put_prop_string(ctx, -2, "find_resolver");

  duk_push_c_lightfunc(ctx, strips_push_module, 1, 1, 0);
  duk_put_prop_string(ctx, -2, "find_module");

  duk_push_object(ctx);
  duk_put_prop_string(ctx, -2, "types");

  duk_put_prop_string(ctx, -2, "strips");
}

strips_ret_t strips_initialize(duk_context *ctx) {

  duk_push_global_stash(ctx);
  if (duk_has_prop_string(ctx, -1, "strips")) {
    duk_pop(ctx);
    return STRIPS_ALREADY_INITIALIZED;
  }

  strips_initialize_stash(ctx);
  duk_pop(ctx);
  strips_commonjs_init(ctx);
  duk_console_init(ctx, DUK_CONSOLE_FLUSH);
  duk_module_add_lstr(ctx, "util", (const char *)util_js, util_js_len);

  strips_set_module_resolver(ctx, "module", cjs_resolve_module,
                             cjs_load_module);

  // strips_set_module_resolver(ctx, "file", cjs_resolve_file, cjs_load_file);

  // strips_path_init(ctx);
  // strips_prompt_init(ctx);

  return STRIPS_OK;
}

duk_ret_t strips_eval_path(duk_context *ctx, const char *path, char **err) {
  char *buffer = NULL;
  int len = 0;
  int c = 0;

  if (!sy_path_is_abs(path)) {
    path = sy_path_abs(path, NULL, 0);
    c = 1;
  }

  int size = cs_file_size(path);
  buffer = duk_push_fixed_buffer(ctx, size);
  if (!(buffer = cs_read_file(path, buffer, size, &len))) {

    if (c)
      free((char *)path);
    if (err) {
      // dukext_err_t *e = (dukext_err_t *)malloc(sizeof(dukext_err_t));
      // e->message = strdup("file not found");
      *err = "file not found";
    }
    return DUK_EXEC_ERROR;
  }

  duk_buffer_to_string(ctx, -1);

  duk_ret_t ret = strips_commonjs_eval_main(ctx, path);

  if (c)
    free((char *)path);

  if (ret == DUK_EXEC_ERROR && err) {
    if (duk_get_prop_string(ctx, -1, "stack")) {
      duk_replace(ctx, -2);
    } else {
      duk_pop(ctx);
    }
    // dukext_err_t *e = (dukext_err_t *)malloc(sizeof(dukext_err_t));
    // e->message = strdup(duk_require_string(ctx, -1));
    *err = duk_require_string(ctx, -1);
  }

  return ret;
}
duk_ret_t strips_eval_script(duk_context *ctx, const char *script,
                             const char *path, char **err) {

  duk_push_string(ctx, script);

  duk_ret_t ret = strips_commonjs_eval_main(ctx, path);

  if (ret == DUK_EXEC_ERROR && err) {
    if (duk_get_prop_string(ctx, -1, "stack")) {
      duk_replace(ctx, -2);
    } else {
      duk_pop(ctx);
    }

    *err = duk_require_string(ctx, -1);
  }

  return ret;
}