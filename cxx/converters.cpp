//#include "object_p.hpp"
#include <strips++/any.hpp>
#include <strips++/callable.hpp>
#include <strips++/converters.hpp>
#include <strips++/function.hpp>
#include <strips++/object.hpp>
#include <strips++/reference.hpp>
#include <strips++/vm.hpp>

namespace strips {

void to_duktape(duk_context *ctx, const char *str) {
  duk_push_string(ctx, str);
}

void to_duktape(duk_context *ctx, const std::string &str) {
  duk_push_string(ctx, str.c_str());
}

void from_duktape(duk_context *ctx, duk_idx_t idx, std::string &str) {
  if (duk_is_string(ctx, idx)) {
    const char *s = duk_get_string(ctx, idx);
    str = std::string(s);
  } else if (duk_is_buffer_data(ctx, idx)) {
    duk_dup(ctx, idx);
    duk_buffer_to_string(ctx, idx);
    const char *s = duk_get_string(ctx, idx);
    str = std::string(s);
    duk_pop(ctx);
  } else {
    throw std::runtime_error("not a string");
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
  return 0;
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
  auto *call =
      new details::Callable<std::function<duk_ret_t(VM &)>>(std::move(fn));
  call->push(ctx);
}

/*void to_duktape(duk_context *ctx, std::function<duk_ret_t(const VM &)> fn) {
  auto *call = new details::Callable<std::function<duk_ret_t(const VM
&)>>(std::move(fn)); call->push(ctx);
}*/

void to_duktape(duk_context *ctx, duk_c_function fn) {
  duk_push_c_function(ctx, fn, DUK_VARARGS);
}

void to_duktape(duk_context *ctx, const Object &o) { o.push(); }

void to_duktape(duk_context *ctx, const Function &o) { o.push(); }

void from_duktape(duk_context *ctx, duk_idx_t idx, Function &fn) {
  duk_dup(ctx, idx);
  if (!duk_is_callable(ctx, -1)) {
    duk_pop(ctx);
    throw std::runtime_error("not a callable");
  }
  fn.set_ctx(ctx);
  fn.set_ref(duk_ref(ctx));
}

void from_duktape(duk_context *ctx, duk_idx_t idx, Object &o) {
  duk_dup(ctx, idx);
  o.set_ctx(ctx);
  o.set_ref(duk_ref(ctx));
}

void from_duktape(duk_context *ctx, duk_idx_t idx, Array &o) {
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

void to_duktape(duk_context *ctx, const std::map<std::string, Any> &v) {
  duk_push_object(ctx);
  for (auto &a : v) {
    a.second.push_duktape(ctx);
    duk_put_prop_string(ctx, -2, a.first.c_str());
  }
}

} // namespace strips