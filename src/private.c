#include "private.h"

#define STRIPS_GUARD                                                           \
  duk_push_global_stash(ctx);                                                  \
  if (!duk_has_prop_string(ctx, -1, "strips")) {                               \
    return false;                                                              \
  }                                                                            \
  duk_get_prop_string(ctx, -1, "strips");                                      \
  duk_remove(ctx, -2);

bool strips_push_entry(duk_context *ctx, const char *name) {
  STRIPS_GUARD

  duk_dup(ctx, -2);
  duk_put_prop_string(ctx, -2, name);
  duk_pop_2(ctx);

  return true;
}

bool strips_get_entry(duk_context *ctx, const char *name) {
  STRIPS_GUARD
  bool ret = true;

  if (!duk_has_prop_string(ctx, -1, name)) {
    duk_pop(ctx);
    return false;
  }

  duk_get_prop_string(ctx, -1, name);

  duk_remove(ctx, -2);

  return true;
}

bool strips_has_entry(duk_context *ctx, const char *name) {
  STRIPS_GUARD

  duk_bool_t b = duk_has_prop_string(ctx, -1, name);
  duk_pop(ctx);
  return b;
}

duk_ret_t strips_push_module(duk_context *ctx) {
  const char *name = duk_require_string(ctx, 0);

  if (!strips_get_entry(ctx, "modules")) {
    return 0;
  }

  if (!duk_has_prop_string(ctx, -1, name)) {
    return 0;
  }

  duk_get_prop_string(ctx, -1, name);

  return 1;
}