#include <strips++/object.hpp>
#include <strips++/strips++.hpp>
#include <strips/utils.h>
#include "object_p.hpp"

namespace strips {



Object::Object(duk_context *ctx, int ref)
    : d(new internal::ObjectPrivate(ctx, ref)) {}

Object::Object() : d(new internal::ObjectPrivate(NULL, 0)) {}

Object::Object(Object &&o) : d(std::move(o.d)) {}

duk_context *Object::ctx() const { return d->ctx; }

Object::~Object() {}

void Object::push_self() const { duk_push_ref(d->ctx, d->ref); }

bool Object::has(const std::string &name) {
  push_self();
  bool ret = duk_has_prop_string(ctx(), -1, name.c_str());
  duk_pop(ctx());
  return ret;
}

void Object::remove(const std::string &name) {
  push_self();
  duk_del_prop_string(ctx(), -1, name.c_str());
  duk_pop(ctx());
}

void Object::push() const { duk_push_ref(ctx(), d->ref); }

Object Object::clone() const {
  duk_get_global_string(ctx(),"Object");
  duk_push_string(ctx(), "create");
  duk_push_ref(ctx(), d->ref);
  duk_pcall_prop(ctx(), -3, 1);
  Object ob(ctx(), duk_ref(ctx()));
  duk_pop(ctx());
  return std::move(ob);
}

std::ostream &operator<<(std::ostream &o, const Object &v) {
  v.push_self();
  duk_json_encode(v.ctx(), -1);
  const char *str = duk_get_string(v.ctx(), -1);
  duk_pop(v.ctx());
  o << std::string(str);
  return o;
}

} // namespace strips
