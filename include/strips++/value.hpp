#pragma once
#include <duktape.h>
#include <strips++/converters.hpp>
#include <strips/utils.h>

namespace strips {

enum class Type {
  String,
  Number,
  Bool,
  Object,
  Array,
  Null,
  Undefined,
  Type,
  Function,
  Buffer
};

class Value {

public:
  template <class T> Value(duk_context *ctx, const T &t) : m_ctx(ctx) {
    to_duktape(ctx, t);
    m_ref = duk_ref(ctx);
  }

  Value(duk_context *ctx, int ref);
  Value(const Value &v);
  Value(const Value &&v);
  ~Value() {}

  Value &operator=(const Value &v);
  Value &operator=(Value &&v);

  Type type() const;
  void push() const;

  template <class T> T as() {
    duk_push_ref(m_ctx, m_ref);
    T v;
    from_duktape(m_ctx, -1, v);
    return std::move(v);
  }

private:
  friend void from_duktape(duk_context *ctx, duk_idx_t idx, Value &o);
  friend class Object;
  friend class VM;
  Value();
  void init_type();
  duk_context *m_ctx = NULL;
  int m_ref = 0;
  Type m_type;
};

template <class F, class... Args> void iterate_vaargs(F f, Args &... args) {
  int x[] = {(f(args), 0)...};
}

class Function {

public:
  Function(duk_context *c) : ctx(c), ref(duk_ref(c)) {}

  template <typename... Args> duk_ret_t call(Args &... args) {

    duk_push_ref(ctx, ref);
    int size = 0;
    iterate_vaargs(
        [&](auto &arg) {
          to_duktape(ctx, arg);
          size++;
        },
        args...);
    duk_ret_t ret = duk_pcall(ctx, size);

    return ret;
  }

private:
  duk_context *ctx;
  int ref;
};

} // namespace strips