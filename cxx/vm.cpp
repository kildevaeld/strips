#include <strips++/converters.hpp>
#include <strips++/vm.hpp>
#include <strips/modules.h>
#include <strips/strips.h>
#include <strips/utils.h>

namespace strips {

namespace internal {
class VMPrivate {
public:
  VMPrivate(duk_context *c = NULL, bool o = false) : ctx(c) {
    if (c == NULL) {
      ctx = duk_create_heap_default();
      owner = true;
    } else {
      owner = o;
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

Reference VM::eval_path(const std::string &path) {
  char *err = NULL;
  strips_eval_path(ctx(), path.c_str(), &err);
  if (err) {
    throw std::runtime_error(err);
  }
  Reference v(ctx());
  duk_pop(ctx());
  return std::move(v);
}
Reference VM::eval_script(const std::string &script, const std::string &path) {
  char *err = NULL;
  strips_eval_script(ctx(), script.c_str(), path.c_str(), &err);

  if (err) {
    throw std::runtime_error(err);
  }

  Reference v(ctx(), -1);

  duk_pop(ctx());
  return std::move(v);
}
Reference VM::eval(const std::string &script) {
  duk_eval_string(ctx(), script.c_str());
  return std::move(pop<Reference>());
}

duk_context *VM::ctx() const { return d->ctx; }

Object VM::object() const {
  duk_push_object(ctx());
  Object o(ctx(), -1);
  duk_pop(ctx());
  return std::move(o);
}

Array VM::array() const {
  duk_push_array(ctx());
  Array a(ctx(), -1);
  duk_pop(ctx());
  return std::move(a);
}

Object VM::global() const {
  duk_push_global_object(ctx());
  Object o(ctx(), -1);
  duk_pop(ctx());
  return std::move(o);
}

Object VM::stash() const {
  duk_push_global_stash(ctx());
  Object o(ctx(), -1);
  duk_pop(ctx());
  return std::move(o);
}

Object VM::require(const std::string &name) const {
  return global().call<Object>("require", name);
}

const VM &VM::dump() const {
  duk_dump_context_stdout(d->ctx);
  return *this;
}

duk_size_t VM::top() const { return duk_get_top(d->ctx); }

VM &VM::pop(int count) {
  if (count == 0)
    return *this;
  duk_pop_n(ctx(), count);

  return *this;
}

VM &VM::remove(duk_idx_t idx) {
  duk_remove(ctx(), idx);
  return *this;
}

void VM::register_module(const std::string &name,
                         std::function<duk_ret_t(VM &vm)> fn) {

  to_duktape(ctx(), fn);

  duk_idx_t idx = duk_normalize_index(ctx(), -1);
  if (duk_module_add_fn_idx(ctx(), name.c_str(), idx) != STRIPS_OK) {
    pop();
    throw std::runtime_error("could register module");
  }

  pop();
}

void VM::set_ctx(duk_context *ctx, bool own) {
  if (d->ctx && d->owner) {
    duk_destroy_heap(d->ctx);
    d->ctx = NULL;
  }
  d->ctx = ctx;
  d->owner = own;
  strips_initialize(d->ctx);
}

} // namespace strips
