#include <string>
#include <strips++/array.hpp>
#include <strips++/object.hpp>
#include <strips++/reference.hpp>
#include <strips/utils.h>

namespace strips {

static Type get_type(duk_context *ctx);

std::ostream &operator<<(std::ostream &s, const Type &type) {
  switch (type) {
  case Type::Bool:
    s <<  std::string("Bool");
    break;
  case Type::Undefined:
    s << std::string("Undefined");
    break;
  case Type::Null:
    s << std::string("Null");
    break;
  case Type::Number:
    s << std::string("Number");
    break;
  case Type::String:
     s << std::string("String");
    break;
  case Type::Function:
    s << std::string("Function");
    break;
  case Type::Buffer: {
    s << std::string("Buffer");

  } break;
  case Type::Object: {
     s << std::string("Object");
  } break;
  case Type::Array: {
    s << std::string("Array");
  } break;
  }
  return s;
}

namespace internal {
class ReferencePrivate {
public:
  ReferencePrivate() {}
  ReferencePrivate(duk_context *c, duk_idx_t idx) : ctx(c) {
    duk_dup(ctx, idx);
    ref = duk_ref(ctx);
  }
  ReferencePrivate(duk_context *c) : ctx(c) {}
  ~ReferencePrivate() {
    if (ctx && ref) {
      duk_unref(ctx, ref);
      ref = 0;
    }
  }
  ReferencePrivate *clone() const;
  bool valid() const;
  duk_context *ctx = NULL;
  int ref = 0;
};

ReferencePrivate *ReferencePrivate::clone() const {
 
  duk_push_ref(ctx, ref);
  duk_idx_t  nidx = duk_normalize_index(ctx, -1);
  auto type = get_type(ctx);
   std::cout << "clone " << type  << std::endl;
  switch (type) {
  case Type::Bool:
  case Type::Undefined:
  case Type::Null:
  case Type::Number:
  case Type::String:
  case Type::Function:
    duk_dup(ctx, -1);
    break;
  case Type::Buffer: {
    duk_size_t size;
    void *o = duk_get_buffer(ctx, -1, &size);
    void *out = duk_push_fixed_buffer(ctx, size);
    memcpy(out, o, size);

  } break;
  case Type::Object: {
    duk_idx_t oidx = duk_push_object(ctx);
    duk_enum(ctx, -2, DUK_ENUM_OWN_PROPERTIES_ONLY);
    while (duk_next(ctx, -1, 1)) {
      const char *k = duk_get_string(ctx, -2);
      duk_put_prop_string(ctx, oidx, k);
      duk_pop(ctx);
    }
    duk_pop(ctx);
  } break;
  case Type::Array: {
    duk_idx_t aidx = duk_push_array(ctx);
    int len = duk_get_length(ctx, nidx);
    for (int i = 0; i < len; i++) {
      duk_get_prop_index(ctx, nidx, i);
      duk_put_prop_index(ctx, aidx, i);
    }
  } break;  
  }
  
  auto out = new ReferencePrivate(ctx, -1);
  duk_pop_2(ctx);
  return out;
}
bool ReferencePrivate::valid() const { return ctx != NULL && ref > 0; }
} // namespace internal
Reference::Reference() : ptr(new internal::ReferencePrivate()) {}
Reference::Reference(duk_context *ctx)
    : ptr(new internal::ReferencePrivate(ctx)) {}
Reference::Reference(duk_context *ctx, duk_idx_t idx)
    : ptr(new internal::ReferencePrivate(ctx, idx)) {}

Reference::Reference(const Reference &o) : ptr(o.ptr->clone()) {}
Reference::Reference(Reference &&o) : ptr(std::move(o.ptr)) {}

Reference &Reference::operator=(const Reference &o) {
  if (this != &o) {
    ptr.reset(o.ptr->clone());
  }
  return *this;
}
Reference &Reference::operator=(Reference &&o) {
  if (this != &o) {
    ptr.swap(o.ptr);
  }
  return *this;
}

Reference::~Reference() {}

duk_context *Reference::ctx() const { return ptr->ctx; }
bool Reference::valid() const { return ptr->valid(); }

int Reference::ref() const { return ptr->ref; }

void Reference::set_ref(int ref) {

  if (ptr->ref) {
    duk_unref(ptr->ctx, ptr->ref);
  }
  ptr->ref = ref;
}

void Reference::push() const {
  if (!ptr->valid()) {
    throw std::runtime_error("cannot push: reference is invalid");
  }

  duk_push_ref(ptr->ctx, ptr->ref);
}

void Reference::unref() {
  this->set_ref(0);
}

static Type get_type(duk_context *ctx) {

  switch (duk_get_type(ctx, -1)) {
  case DUK_TYPE_BOOLEAN:
    return Type::Bool;
  case DUK_TYPE_STRING:
    return Type::String;
  case DUK_TYPE_NUMBER:
    return Type::Number;
  case DUK_TYPE_NULL:
    return Type::Null;
  case DUK_TYPE_UNDEFINED:
    return Type::Undefined;
  case DUK_TYPE_BUFFER:
    return Type::Buffer;
  case DUK_TYPE_OBJECT: {
    if (duk_is_function(ctx, -1)) {
      return Type::Function;
    } else if (duk_is_array(ctx, -1)) {
      return Type::Array;
    } else {
      return Type::Object;
    }
  }
  }

  return Type::Invalid;
}

Type Reference::type() const {

  push();
  Type type = get_type(ctx());
  duk_pop(ctx());
  return type;
}

void Reference::set_ctx(duk_context *ctx) { ptr->ctx = ctx; }

std::ostream &operator<<(std::ostream &s, const Reference &r) {
  r.push();
  auto ctx = r.ctx();
  auto type = get_type(ctx);
  switch (type) {
  case Type::Bool:
    s << std::string(duk_get_boolean(ctx, -1) ? "true" : "false");
    break;
  case Type::Undefined:
    s << std::string("undefined");
    break;
  case Type::Null:
    s << std::string("null");
    break;
  case Type::Number:
    s << std::to_string(duk_get_number(ctx, -1));
    break;
  case Type::String:
    s << std::string(duk_get_string(ctx, -1));
    break;
  case Type::Function:
    s << std::string("[Function]");
    break;
  case Type::Buffer: {
    s << std::string("[Buffer]");

  } break;
  case Type::Object: {
    /*d uk_idx_t oidx = duk_push_object(ctx);
     duk_enum(ctx, -2, DUK_ENUM_OWN_PROPERTIES_ONLY);
     while (duk_next(ctx, -1, 1)) {
       const char *k = duk_get_string(ctx, -2);
       duk_put_prop_string(ctx, oidx, k);
       duk_pop(ctx);
     }
     duk_pop(ctx);*/
    s << r.as<Object>(); // std::string("[Object]");
  } break;
  case Type::Array: {
    s << r.as<Array>();
  } break;
  }

  duk_pop(ctx);
  return s;
}

} // namespace strips