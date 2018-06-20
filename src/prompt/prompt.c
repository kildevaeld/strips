#include <strips/modules.h>
#include <strips/prompt/prompt.h>
#include <syrup/array-list.h>
#include <syrup/form.h>
#include <syrup/term.h>

static bool array_to_array_list(duk_context *ctx, duk_idx_t idx,
                                sy_array_t *a) {
  if (!duk_is_array(ctx, idx))
    return false;
  int len = duk_get_length(ctx, idx);

  for (int i = 0; i < len; i++) {
    duk_get_prop_index(ctx, idx, i);
    if (!duk_is_string(ctx, -1))
      return false;
    sy_array_append(a, (void *)duk_get_string(ctx, -1));
    duk_pop(ctx);
  }

  return true;
}

static bool get_config(duk_context *ctx, duk_idx_t idx,
                       struct sy_term_form_field_cfg *cfg, char **err) {

  int row, col;
  sy_term_enable_raw_mode();
  sy_term_cursor_pos_get(&row, &col);
  sy_term_disable_raw_mode();

  cfg->row = row;
  cfg->col = col;

  if (!duk_is_object(ctx, idx))
    return true;

  if (duk_has_prop_string(ctx, idx, "message")) {
    duk_get_prop_string(ctx, idx, "message");
    cfg->msg = duk_require_string(ctx, -1);
    duk_pop(ctx);
  }

  if (duk_has_prop_string(ctx, idx, "clear")) {
    duk_get_prop_string(ctx, idx, "clear");
    cfg->clear = duk_get_boolean(ctx, -1);
    duk_pop(ctx);
  }

  if (duk_has_prop_string(ctx, idx, "row")) {
    duk_get_prop_string(ctx, idx, "row");
    cfg->row = duk_require_int(ctx, -1);
    duk_pop(ctx);
  }

  if (duk_has_prop_string(ctx, idx, "col")) {
    duk_get_prop_string(ctx, idx, "col");
    cfg->col = duk_require_int(ctx, -1);
    duk_pop(ctx);
  }

  return true;
}

static duk_ret_t de_prompt_list(duk_context *ctx) {

  sy_term_form_select_cfg cfg;
  memset(&cfg, 0, sizeof(cfg));

  get_config(ctx, 0, (struct sy_term_form_field_cfg *)&cfg, NULL);

  cfg.max_selected = 1;
  cfg.selected = "◉ ";
  cfg.unselected = "◯ ";

  sy_array_t *choices = sy_array_new((sy_array_comparator_fn)strcmp);

  if (duk_is_undefined(ctx, 0)) {
    duk_type_error(ctx, "needs an object");
  } else if (duk_is_string(ctx, 0)) {
    if (!duk_is_array(ctx, 1)) {
      sy_array_free(choices);
      duk_type_error(ctx, "2end args must be an array of strings");
    }
    cfg.msg = (char *)duk_require_string(ctx, 0);
    if (!array_to_array_list(ctx, 1, choices)) {
      sy_array_free(choices);
      duk_type_error(ctx, "invalid array");
    }
  } else if (duk_is_object(ctx, 0)) {

    if (!duk_has_prop_string(ctx, 0, "choices")) {
      sy_array_free(choices);
      duk_type_error(ctx, "must provide choices array");
    }

    duk_get_prop_string(ctx, 0, "choices");
    if (!array_to_array_list(ctx, -1, choices)) {
      sy_array_free(choices);
      duk_type_error(ctx, "invalid array");
    }
    duk_pop(ctx);

    if (duk_has_prop_string(ctx, 0, "max")) {
      duk_get_prop_string(ctx, 0, "max");
      cfg.max_selected = duk_get_number_default(ctx, -1, 1);
      duk_pop(ctx);
    }

    if (duk_has_prop_string(ctx, 0, "min")) {
      duk_get_prop_string(ctx, 0, "min");
      cfg.min_selected = duk_get_number_default(ctx, -1, 1);
      duk_pop(ctx);
    }
  }

  sy_list_edit_res_t *res = sy_term_form_select(
      &cfg, (char **)sy_array_raw(choices), sy_array_len(choices));

  if (!res) {
    sy_array_free(choices);
    return 0;
  }

  if (cfg.max_selected <= 1) {
    if (res->len > 0) {
      duk_push_string(ctx,
                      (const char *)sy_array_get(choices, res->indexes[0]));
    } else {
      duk_push_null(ctx);
    }
    return 1;
  } else {
    duk_push_array(ctx);
    for (int i = 0; i < res->len; i++) {
      duk_push_string(ctx,
                      (const char *)sy_array_get(choices, res->indexes[i]));
      duk_put_prop_index(ctx, -2, i);
    }
  }

  sy_array_free(choices);
  sy_term_list_res_free(res);

  return 1;
}

static duk_ret_t de_prompt_confirm(duk_context *ctx) {

  sy_term_form_confirm_cfg cfg;
  memset(&cfg, 0, sizeof(sy_term_form_confirm_cfg));
  get_config(ctx, 0, (struct sy_term_form_field_cfg *)&cfg, NULL);

  if (duk_is_undefined(ctx, 0)) {
    duk_type_error(ctx, "must provide message string or object");
  }

  if (duk_is_string(ctx, 0)) {
    cfg.msg = (char *)duk_require_string(ctx, 0);
  } else if (duk_is_object(ctx, 0)) {
    if (duk_has_prop_string(ctx, 0, "defaults")) {
      duk_get_prop_string(ctx, 0, "defaults");
      cfg.defaults = duk_require_boolean(ctx, -1);
      duk_pop(ctx);
    }
  }

  bool ret = sy_term_form_confirm(&cfg);
  duk_push_boolean(ctx, ret);

  return 1;
}

static duk_ret_t de_prompt_input(duk_context *ctx) {

  sy_term_form_input_cfg cfg;
  memset(&cfg, 0, sizeof(sy_term_form_input_cfg));

  get_config(ctx, 0, (struct sy_term_form_field_cfg *)&cfg, NULL);
  cfg.echo = duk_get_current_magic(ctx);

  if (duk_is_undefined(ctx, 0)) {
    duk_type_error(ctx, "must provide message string or object");
  }

  if (duk_is_string(ctx, 0)) {
    cfg.msg = (char *)duk_require_string(ctx, 0);
  } else if (duk_is_object(ctx, 0)) {
    if (duk_has_prop_string(ctx, 0, "defaults")) {
      duk_get_prop_string(ctx, 0, "defaults");
      cfg.defaults = duk_require_string(ctx, -1);
      duk_pop(ctx);
    }
  }

  const char *ans = sy_term_form_input(&cfg);
  if (ans) {
    duk_push_string(ctx, ans);
    free(ans);
    return 1;
  }

  return 0;
}

static duk_ret_t duk_prompt_module(duk_context *ctx) {
  duk_push_object(ctx);

  duk_push_c_function(ctx, de_prompt_list, 2);
  duk_put_prop_string(ctx, -2, "list");

  duk_push_c_function(ctx, de_prompt_confirm, 2);
  duk_put_prop_string(ctx, -2, "confirm");

  duk_push_c_function(ctx, de_prompt_input, 1);
  duk_set_magic(ctx, -1, 1);
  duk_put_prop_string(ctx, -2, "input");

  duk_push_c_function(ctx, de_prompt_input, 1);
  duk_set_magic(ctx, -1, 0);
  duk_put_prop_string(ctx, -2, "password");

  return 1;
}

void strips_prompt_init(duk_context *ctx) {
  duk_module_add_fn(ctx, "prompt", duk_prompt_module);
}