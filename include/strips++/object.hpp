#pragma once
#include <duktape.h>
#include <functional>
#include <memory>
#include <string>
#include <strips/utils.h>
#include <strips++/value.hpp>

namespace strips {

class VM;

namespace internal {
class ObjectPrivate;
}

class Object {
  Object(duk_context *ctx, int ref);

public:
  Object(Object &&);
  Object(const Object &) = delete;
  ~Object();

  duk_context *ctx() const;

  template <class T> void set(const std::string &name, const T &v) {
    push_self();
    to_duktape(ctx(), v);
    duk_put_prop_string(ctx(), -2, name.c_str());
    duk_pop(ctx());
  }

  template <class T = Value> T get(const std::string &name) {

    push_self();
    T v;
    if (!duk_has_prop_string(ctx(), -1, name.c_str())) {
      throw std::runtime_error("no name");
    }

    duk_get_prop_string(ctx(), -1, name.c_str());
    from_duktape(ctx(), -1, v);
    duk_pop_2(ctx());
    return std::move(v);
  }

  template <typename... Args> duk_ret_t call(const std::string &prop, Args &... args) {

    push_self();
    duk_idx_t idx = duk_normalize_index(ctx(), -1);
    duk_push_string(ctx(), prop.c_str());
    int size = 0;
    iterate_vaargs(
        [&](auto &arg) {
          to_duktape(ctx(), arg);
          size++;
        },
        args...);

    duk_ret_t ret = duk_pcall_prop(ctx(), idx, size);
    duk_remove(ctx(), -2);
    return ret;
  }


  bool has(const std::string &name);

  void remove(const std::string &name);

  void push() const;

  Object clone() const;

   friend std::ostream &operator<<(std::ostream &o, const Object &v);

private:
  friend class VM;
  friend class Value;
  friend void from_duktape(duk_context *ctx, duk_idx_t idx, Object &o);
  Object();

  void push_self() const;
  std::unique_ptr<internal::ObjectPrivate> d;
}; // namespace strips

} // namespace strips