//#include "object_p.hpp"
#include <strips++/converters.hpp>
#include <strips++/object.hpp>
#include <strips++/reference.hpp>
#include <strips++/vm.hpp>

namespace strips {

void to_duktape(duk_context *ctx, const std::string &str) {
  duk_push_string(ctx, str.c_str());
}

void from_duktape(duk_context *ctx, duk_idx_t idx, std::string &str) {
  if (duk_is_string(ctx, idx)) {
    const char *s = duk_get_string(ctx, idx);
    str = std::string(s);
  }
}

/*void to_duktape(duk_context *ctx, const bool &str) {
  duk_push_boolean(ctx, str);
}

void from_duktape(duk_context *ctx, duk_idx_t idx, bool &str) {
  if (duk_is_boolean(ctx, idx)) {
    duk_bool_t s = duk_get_boolean(ctx, idx);
    str = s;
  }
}*/

struct fn_bag {
  std::function<duk_ret_t(VM &)> fn;
};

static duk_ret_t fn_fin(duk_context *ctx) {
  if (duk_has_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("bag"))) {
    duk_get_prop_string(ctx, -0, DUK_HIDDEN_SYMBOL("bag"));
    fn_bag *bag = (fn_bag *)duk_get_pointer(ctx, -1);
    delete bag;
  }
}

static duk_ret_t fn_apply(duk_context *ctx) {
  duk_push_current_function(ctx);
  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("bag"));
  fn_bag *bag = (fn_bag *)duk_get_pointer(ctx, -1);
  duk_pop_2(ctx);

  VM strips(ctx);
  duk_ret_t ret;
  try {
    ret = bag->fn(strips);
  } catch (const std::runtime_error &e) {
    duk_type_error(ctx, e.what());
  } catch (...) {
    duk_type_error(ctx, "unknon error");
  }
  return ret;
}

void to_duktape(duk_context *ctx, std::function<duk_ret_t(VM &)> fn) {

  auto bag = new fn_bag();
  bag->fn = fn;

  duk_push_c_function(ctx, fn_apply, DUK_VARARGS);
  duk_push_pointer(ctx, bag);
  duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("bag"));
  duk_push_c_function(ctx, fn_fin, 1);
  duk_set_finalizer(ctx, -2);
}

void to_duktape(duk_context *ctx, duk_c_function fn) {
  duk_push_c_function(ctx, fn, DUK_VARARGS);
}

void to_duktape(duk_context *ctx, const Object &o) { o.push(); }

void from_duktape(duk_context *ctx, duk_idx_t idx, Object &o) {
  duk_dup(ctx, idx);
  o.set_ctx(ctx);
  o.set_ref(duk_ref(ctx));
}

void to_duktape(duk_context *ctx, const Reference &o) { o.push(); }

void from_duktape(duk_context *ctx, duk_idx_t idx, Reference &o) {
  duk_dup(ctx, idx);
  o.set_ref(duk_ref(ctx));
  o.set_ctx(ctx);
}

} // namespace strips