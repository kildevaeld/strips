#include <strips/strips.h>
#include <strips/utils.h>


static void strips_setup_ref(duk_context *ctx) {
  duk_push_heap_stash(ctx);

  // Create a new array with one `0` at index `0`.
  duk_push_array(ctx);
  duk_push_int(ctx, 0);
  duk_put_prop_index(ctx, -2, 0);
  // Store it as "refs" in the heap stash
  duk_put_prop_string(ctx, -2, "refs");

  duk_pop(ctx);
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
  duk_put_prop_string(ctx, -2, "strips");
  
}

strips_ret_t strips_initialize(duk_context *ctx) {

  duk_push_global_stash(ctx);
  if (duk_has_prop_string(ctx, -1, "strips")) {
    return STRIPS_ALREADY_INITIALIZED;
  }

  strips_initialize_stash(ctx);
  duk_pop(ctx);

  return STRIPS_OK;
}