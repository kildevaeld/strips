#include <strips++/converters.hpp>
#include <strips++/vm.hpp>
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

Reference VM::eval_path(const std::string &path)  {
  char *err = NULL;
  strips_eval_path(ctx(), path.c_str(), &err);
  if (err) {
    throw std::runtime_error(err);
  }
  Reference v(ctx());
  duk_pop(ctx());
  return std::move(v);
}
Reference VM::eval_script(const std::string &script,
                          const std::string &path)  {
  char *err = NULL;
  strips_eval_script(ctx(), script.c_str(), path.c_str(), &err);
  if (err) {
    throw std::runtime_error(err);
  }
  Reference v(ctx());
  duk_pop(ctx());
  return std::move(v);
}
Reference VM::eval(const std::string &script)  {
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

static duk_ret_t class_ctor(duk_context *ctx) { return 0; }

/*Function VM::ctor(const Object &o) const {

  if (o.has("constructor")) {
    o.get("constructor").push();
    o.remove("constructor");
  } else {
    duk_push_c_function(ctx(), class_ctor, 0);
  }

  o.push();

  duk_put_prop_string(ctx(), -2, "prototype");

  Function fn(ctx(), -1);

  duk_pop(ctx());

  return std::move(fn);
}*/

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

 VM &VM::pop(int count)  {
  if (count == 0)
    return *this;
  duk_pop_n(ctx(), count);

  return *this;
}

 VM &VM::remove(duk_idx_t idx)  {
  duk_remove(ctx(), idx);
  return *this;
}

} // namespace strips
