#pragma once
#include <strips++/reference.hpp>
#include <strips++/function.hpp>
#include <strips++/object.hpp>
namespace strips {

class Function : public Object {
  typedef duk_ret_t (*Handle)(duk_context *ctx, duk_idx_t nargs);

public:
  Function() : Object() {}
  Function(duk_context *ctx) : Object(ctx) {}
  Function(duk_context *ctx, duk_idx_t idx) : Object(ctx, idx) {}
  Function(const Function &o) : Object(o) {}
  Function(Function &&o) : Object(std::move(o)) {}

  template <class T = Reference, typename... Args>
  T call(const Reference &ref, Args &... args) {
    push();
    ref.push();
    return call2(duk_pcall_method, std::forward<Args>(args)...);
  }

  template <class T = Object, typename... Args> T construct(Args &... args) {
    push();
    return call2((Handle)duk_pnew, std::forward<Args>(args)...);
  }

  template <class T = Reference, typename... Args>
  T operator()(Args &... args) {
    push();
    return call2(duk_pcall, std::forward<Args>(args)...);
  }

  template <class T = Reference, typename... Args>
  T operator()(Args &... args) const {
    push();
    return call2(duk_pcall, std::forward<Args>(args)...);
  }

private:
  template <class T = Object, typename... Args>
  T call2(Handle hnd, const Args &... args) {

    int size = 0;
    iterate_vaargs(
        [&](auto &arg) {
          to_duktape(ctx(), arg);
          size++;
        },
        args...);

    duk_ret_t ret = hnd(ctx(), size);

    if (ret != DUK_EXEC_SUCCESS) {
      const char *msg = NULL;
      if (duk_has_prop_string(ctx(), -1, "stack")) {
        duk_get_prop_string(ctx(), -1, "stack");
        msg = duk_get_string(ctx(), -1);
        duk_pop(ctx());
      } else {
          duk_get_prop_string(ctx(), -1, "message");
        msg = duk_get_string(ctx(), -1);
        duk_pop(ctx());
      }
      throw std::runtime_error(msg);
    }

    T v;
    from_duktape(ctx(), -1, v);
    duk_pop(ctx());
    return std::move(v);
  }

private:
  friend void from_duktape(duk_context *ctx, duk_idx_t idx, Function &o);

};

} // namespace strips