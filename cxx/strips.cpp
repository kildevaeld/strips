#include <strips++/converters.hpp>
#include <strips++/strips++.hpp>
#include <strips/strips.h>
#include <strips/utils.h>

namespace strips {

namespace internal {
class VMPrivate {
public:
  VMPrivate(duk_context *c = NULL) : ctx(c) {
    if (c == NULL) {
      ctx = duk_create_heap_default();
      owner = true;
    }
    strips_initialize(ctx);
  }
  ~VMPrivate() {
    if (owner)
      duk_destroy_heap(ctx);
    ctx = NULL;
  }
  duk_context *ctx = NULL;
  bool owner = false;
};
} // namespace internal

VM::VM(duk_context *ctx) : d(new internal::VMPrivate(ctx)) {}

VM::~VM() {}

duk_context *VM::ctx() const { return d->ctx; }

Object VM::object(duk_idx_t idx) const {
  duk_dup(ctx(), idx);
  Object o(ctx(), duk_ref(ctx()));
  return std::move(o);
}

Object VM::object() const {
  duk_push_object(ctx());
  Object o(ctx(), duk_ref(ctx()));
  return std::move(o);
}
Object VM::global() const {
  duk_push_global_object(ctx());
  int ref = duk_ref(ctx());
  Object o(ctx(), ref);
  return std::move(o);
}

Object VM::stash() const {
  duk_push_global_stash(ctx());
  int ref = duk_ref(ctx());
  Object o(ctx(), ref);
  return std::move(o);
}

void VM::dump() const { duk_dump_context_stdout(d->ctx); }

duk_size_t VM::top() const { return duk_get_top(d->ctx); }

} // namespace strips
