#pragma once
#include <duktape.h>
#include <strips/utils.h>
//#include <strips++/vm.hpp>

namespace strips {

class Callable {

public:
  virtual ~Callable() {}

  virtual duk_ret_t call(duk_context *vm) const = 0;
  virtual duk_ret_t call(duk_context *vm) = 0;

  void push(duk_context *ctx, duk_idx_t argc = DUK_VARARGS) {
    duk_push_c_function(ctx, Callable::native_call, argc);
    duk_push_pointer(ctx, this);
    duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("fn"));
    duk_push_c_function(ctx, Callable::native_destroy, 1);
    duk_set_finalizer(ctx, -2);
  }

  static duk_ret_t native_destroy(duk_context *ctx) {
    if (duk_has_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("fn"))) {
      duk_get_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("fn"));
      Callable *call = (Callable *)duk_get_pointer(ctx, -1);
      delete call;
      duk_pop(ctx);
    }

    duk_del_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("fn"));

    return 0;
  }

  static duk_ret_t native_call(duk_context *ctx) {

    duk_push_current_function(ctx);

    duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("fn"));

    Callable *call = reinterpret_cast<Callable *>(duk_get_pointer(ctx, -1));

    duk_pop_2(ctx);
    duk_ret_t ret;

    try {
      ret = call->call(ctx);

    } catch (const std::runtime_error &e) {
      duk_type_error(ctx, "could error: %s", e.what());
    } catch (...) {
      duk_type_error(ctx, "error");
    }
    return ret;
  }
};

namespace details {

template <typename T> class Callable : public ::strips::Callable {

public:
  Callable(T &&fn) : m_fn(std::move(fn)) {}
  duk_ret_t call(duk_context *vm) const {
    return m_fn(vm);
    return 0;
  }

  duk_ret_t call(duk_context *vm) {
    return m_fn(vm);
    return 0;
  }

private:
  T m_fn;
};

} // namespace details

} // namespace strips