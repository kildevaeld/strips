#include <strips/types.h>
#include <strips/utils.h>
#include "private.h"

/*bool strips_initialize_types(duk_context *ctx) {

  if (strips_has_entry(ctx, "types")) {
    return false;
  }
  duk_push_object(ctx);
  strips_push_entry(ctx, "types");

  return true;
}*/

bool duk_global_type_register(duk_context *ctx, const char *name) {
  if (!strips_get_entry(ctx, "types")) {
    return false;
  }
  duk_dup(ctx, -2);
  duk_put_prop_string(ctx, -2, name);
  duk_pop_2(ctx);
  return true;
}
bool duk_global_type_unregister(duk_context *ctx, const char *name) {
  if (!strips_get_entry(ctx, "types")) {
    return false;
  }
  duk_del_prop_string(ctx, -1, name);
  return true;
}

bool duk_global_type_get(duk_context *ctx, const char *name) {
  if (!strips_get_entry(ctx, "types")) {
    return false;
  }
  if (!duk_has_prop_string(ctx, -1, name)) {
    return false;
  }
  duk_get_prop_string(ctx, -1, name);
  duk_remove(ctx, -2);
  return true;
}