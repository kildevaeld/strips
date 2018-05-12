#include <strips++/value.hpp>

namespace strips {
Value::Value() {}
Value::Value(duk_context *ctx, int ref) : m_ctx(ctx), m_ref(ref) {}
Value::Value(const Value &v)
    : m_ctx(v.m_ctx), m_ref(v.m_ref), m_type(v.m_type) {}
Value::Value(const Value &&v)
    : m_ctx(v.m_ctx), m_ref(v.m_ref), m_type(v.m_type) {}

// Value::~Value() {}

Value &Value::operator=(const Value &v) {
  if (this != &v) {
    m_ctx = v.m_ctx;
    m_ref = v.m_ref;
    m_type = v.m_type;
  }
  return *this;
}
Value &Value::operator=(Value &&v) {
  if (this != &v) {
    m_ctx = v.m_ctx;
    m_ref = v.m_ref;
    m_type = v.m_type;
    v.m_ctx = NULL;
    v.m_ref = 0;
  }
  return *this;
}

Type Value::type() const { return m_type; }
void Value::push() const { duk_push_ref(m_ctx, m_ref); }

void Value::init_type() {
  duk_push_ref(m_ctx, m_ref);
  switch (duk_get_type(m_ctx, -1)) {
  case DUK_TYPE_BOOLEAN:
    m_type = Type::Bool;
    break;
  case DUK_TYPE_STRING:
    m_type = Type::String;
    break;
  case DUK_TYPE_NUMBER:
    m_type = Type::Number;
    break;
  case DUK_TYPE_NULL:
    m_type = Type::Null;
    break;
  case DUK_TYPE_UNDEFINED:
    m_type = Type::Undefined;
    break;
  case DUK_TYPE_BUFFER:
    m_type = Type::Buffer;
    break;
  case DUK_TYPE_OBJECT: {

    if (duk_is_function(m_ctx, -1)) {
      m_type = Type::Function;
    } else if (duk_is_array(m_ctx, -1)) {
      m_type = Type::Array;
    } else {
      m_type = Type::Object;
    }
  } break;
  }
  duk_pop(m_ctx);
}

} // namespace strips