#pragma once
#include <duktape.h>
#include <memory>
#include <strips++/converters.hpp>
#include <strips++/object.hpp>

namespace strips {

namespace internal {
class VMPrivate;
}

class VM {

public:
  VM(duk_context *ctx = NULL);
  VM(const VM &) = delete;
  ~VM();

  duk_context *ctx() const;

  Object object(duk_idx_t idx) const;
  Object object() const;

  template <class T> void push(const T &v) { to_duktape(ctx(), v); }
  template <class T = Value> T get(duk_idx_t idx = -1) {
    T v;
    from_duktape(ctx(), idx, v);
    return std::move(v);
  }

  template <class T> T pop() {
    T v = get<T>(-1);
    duk_pop(ctx());
    return std::move(v);
  }

  Object global() const;
  Object stash() const;
  void dump() const;

  duk_size_t top() const;

private:
  std::unique_ptr<internal::VMPrivate> d;
};

} // namespace strips