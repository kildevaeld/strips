#include "private.h"
#include <strips/modules.h>

strips_ret_t duk_module_add_fn(duk_context *ctx, const char *name,
                               duk_c_function fn) {

  if (!strips_get_entry(ctx, "modules")) {
    return STRIPS_NOT_INITIALIZED;
  }

  if (duk_has_prop_string(ctx, -1, name)) {
    duk_pop(ctx);
    return STRIPS_DUPLICATE_MODULE;
  }

  duk_push_c_lightfunc(ctx, fn, 0, 0, 0);
  duk_put_prop_string(ctx, -2, name);

  duk_pop(ctx);

  return STRIPS_OK;
}

strips_ret_t duk_module_add_str(duk_context *ctx, const char *name,
                                const char *script) {
  return duk_module_add_lstr(ctx, name, script, strlen(script));
}

strips_ret_t duk_module_add_lstr(duk_context *ctx, const char *name,
                                 const char *buffer, duk_size_t len) {
  if (!strips_get_entry(ctx, "modules")) {
    return STRIPS_NOT_INITIALIZED;
  }

  if (duk_has_prop_string(ctx, -1, name)) {
    duk_pop(ctx);
    return STRIPS_DUPLICATE_MODULE;
  }

  duk_push_string(ctx,
                  "(function(exports,require,module,__filename,__dirname){");

  duk_push_string(ctx, (buffer[0] == '#' && buffer[1] == '!')
                           ? "//"
                           : "");     /* Shebang support. */
  duk_push_lstring(ctx, buffer, len); /* source */
  duk_push_string(
      ctx,
      "\n})"); /* Newline allows module last line to contain a // comment. */
  duk_concat(ctx, 4);
  duk_push_string(ctx, name); // filename
  duk_pcompile(ctx, DUK_COMPILE_EVAL);
  duk_ret_t ret = duk_pcall(ctx, 0);
  if (ret != DUK_EXEC_SUCCESS) {
    duk_get_prop_string(ctx, -1, "stack");
    printf("errr: %s\n", duk_get_string(ctx, -1));
    duk_throw(ctx);
  }
  duk_put_prop_string(ctx, -2, name);

  duk_pop(ctx);

  return STRIPS_OK;
}

duk_bool_t duk_module_has(duk_context *ctx, const char *name) {

  if (!strips_get_entry(ctx, "modules")) {
    return false;
  }

  bool ret = duk_has_prop_string(ctx, -1, name);
  duk_pop(ctx);
  return ret;
}

duk_ret_t strips_set_module_resolver(duk_context *ctx, const char *protocol,
                                     strips_module_resolve_cb resolve,
                                     strips_module_load_cb load) {

  if (!strips_get_entry(ctx, "resolvers")) {
    return false;
  }

  bool ret = true;

  if (duk_has_prop_string(ctx, -1, protocol)) {
    ret = false;
    goto end;
  }

  duk_push_object(ctx);
  duk_push_c_lightfunc(ctx, resolve, 1, 1, 0);
  duk_put_prop_string(ctx, -2, "resolve");
  duk_push_c_lightfunc(ctx, load, 2, 2, 0);
  duk_put_prop_string(ctx, -2, "load");
  duk_push_string(ctx, protocol);
  duk_put_prop_string(ctx, -2, "protocol");
  duk_put_prop_string(ctx, -2, protocol);

end:
  duk_pop(ctx);
  return ret;
}

bool strips_set_module_parser(duk_context *ctx, const char *ext,
                              duk_c_function fn) {
  if (!strips_get_entry(ctx, "parsers")) {
    return false;
  }

  bool ret = true;

  if (duk_has_prop_string(ctx, -1, ext)) {
    ret = false;
    goto end;
  }

  duk_push_c_lightfunc(ctx, fn, 1, 1, 0);
  duk_put_prop_string(ctx, -2, ext);

end:
  duk_pop(ctx);
  return ret;
}

bool strips_get_module_parser(duk_context *ctx, const char *ext) {
  if (!strips_get_entry(ctx, "parsers")) {
    return false;
  }

  bool ret = true;

  if (!duk_has_prop_string(ctx, -1, ext)) {
    return false;
  }
  duk_put_prop_string(ctx, -1, ext);

  return true;
}