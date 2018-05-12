#pragma once
#include <duktape.h>
#include <strips++/converters.hpp>
#include <strips++/reference.hpp>
#include <strips/utils.h>
#include <strips++/utils.hpp>

namespace strips {

/*enum class Type {
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
};*/
/*
class Value : public Reference {

public:
  template <class T> Value(duk_context *ctx, const T &t) : Reference(ctx) {
    to_duktape(ctx, t);
    set_ref(duk_ref(ctx));
  }

  Value(duk_context *ctx, duk_idx_t idx);
  Value(const Value &v);
  Value(const Value &&v);
  virtual ~Value() {}

  // Value &operator=(const Value &v);
  // Value &operator=(Value &&v);

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
};*/




} // namespace strips