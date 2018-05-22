#include <strips/modules.h>
#include <strips/prompt/prompt.h>
#include <syrup/form.h>

static duk_ret_t de_prompt_list(duk_context *ctx) {
  const char *msg = duk_require_string(ctx, 0);

  if (!duk_is_array(ctx, 1)) {
    duk_type_error(ctx, "should be array");
  }

  int len = duk_get_length(ctx, 1);

  duk_enum(ctx, 1, 0);
  char *buf[len];
  while (duk_next(ctx, -1, 1)) {
    buf[duk_to_int(ctx, -2)] = (char *)duk_require_string(ctx, -1);
    duk_pop_2(ctx);
  }
  duk_pop(ctx);

  sy_term_style_t style;
  int i = sy_term_form_list(&style, msg, (const char **)buf, len);

  if (i == -1) {
    duk_push_undefined(ctx);
  } else {
    duk_get_prop_index(ctx, 1, i);
  }

  return 1;
}

static duk_ret_t de_prompt_confirm(duk_context *ctx) {
  const char *msg = duk_require_string(ctx, 0);

  bool clear = duk_get_boolean_default(ctx, 1, false);

  sy_term_style_t style;
  bool ret = sy_term_form_confirm(&style, msg, clear);
  duk_push_boolean(ctx, ret);

  return 1;
}

duk_ret_t de_prompt_input(duk_context *ctx) {
  const char *msg = duk_require_string(ctx, 0);

  sy_term_style_t style;
  char *ret = sy_term_form_prompt(&style,msg);
  duk_push_string(ctx, ret);
  free(ret);

  return 1;
}

duk_ret_t de_prompt_password(duk_context *ctx) {
  const char *msg = duk_require_string(ctx, 0);

  sy_term_style_t style;
  char *ret = sy_term_form_password(&style, msg, NULL);
  duk_push_string(ctx, ret);
  free(ret);

  return 1;
}

static duk_ret_t duk_prompt_module(duk_context *ctx) {
  duk_push_object(ctx);
  duk_push_c_function(ctx, de_prompt_list, 2);
  duk_put_prop_string(ctx, -2, "list");

  duk_push_c_function(ctx, de_prompt_confirm, 2);
  duk_put_prop_string(ctx, -2, "confirm");

  duk_push_c_function(ctx, de_prompt_input, 2);
  duk_put_prop_string(ctx, -2, "input");

  duk_push_c_function(ctx, de_prompt_password, 2);
  duk_put_prop_string(ctx, -2, "password");

  return 1;
}

void strips_prompt_init(duk_context *ctx) {
  duk_module_add_fn(ctx, "prompt", duk_prompt_module);
}