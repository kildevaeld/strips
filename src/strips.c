#include "commonjs.h"
#include "commonjs_module.h"
#include "private.h"
#include <strips/modules.h>
#include <strips/strips.h>
#include <strips/utils.h>
#include "commonjs_file.h"


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
    return STRIPS_ALREADY_INITIALIZED;
  }

  strips_initialize_stash(ctx);
  duk_pop(ctx);
  strips_commonjs_init(ctx);

  strips_set_module_resolver(ctx, "module", cjs_resolve_module,
                             cjs_load_module);

  strips_set_module_resolver(ctx, "file", cjs_resolve_file, cjs_load_file);

  return STRIPS_OK;
}