#pragma once
#include <strips++/function.hpp>
#include <strips++/object.hpp>
#include <strips++/reference.hpp>
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

  Function super() const {
    push();
    duk_get_prop_string(ctx(), -1, "Super");
    Function fn(ctx(), duk_ref(ctx()));
    duk_pop(ctx());
    return std::move(fn);
  }

  Object prototype() const {
    push();
    duk_get_prop_string(ctx(), -1, "prototype");
    Object obj(ctx(), -1);
    duk_pop_2(ctx());
    return std::move(obj);
  }

  void prototype(const ObjectLiteral &m) {
    push();
    to_duktape(ctx(), m);
    duk_put_prop_string(ctx(), -2, "prototype");
    duk_pop(ctx());
  }

  void prototype(const Object &m) {
    push();
    m.push();
    duk_put_prop_string(ctx(), -2, "prototype");
    duk_pop(ctx());
  }

  void inherit(const Function &m) const {
    push();
    if (duk_has_prop_string(ctx(), -1, "Super")) {
      duk_pop(ctx());
      throw std::runtime_error("already have a super");
    }

    duk_get_global_string(ctx(), "Object");
    duk_push_string(ctx(), "create");
    m.prototype().push();
    duk_ret_t ret = duk_pcall_prop(ctx(), -3, 1);
    if (ret != DUK_EXEC_SUCCESS) {
      const char *str = NULL;
      if (duk_has_prop_string(ctx(), -1, "stack")) {
        duk_get_prop_string(ctx(), -1, "stack");
        str = duk_get_string(ctx(), -1);
      } else {
        duk_get_prop_string(ctx(), -1, "message");
        str = duk_get_string(ctx(), -1);
      }
      duk_pop_3(ctx());
      throw std::runtime_error(str);
    }

    duk_put_prop_string(ctx(), -3, "prototype");

    m.push();
    duk_put_prop_string(ctx(), -3, "Super");

    duk_pop_2(ctx());
  }

  bool valid() const override { return type() == Type::Function; }

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