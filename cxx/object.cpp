//#include "object_p.hpp"
#include <strips++/object.hpp>
#include <strips++/vm.hpp>
#include <strips/utils.h>

namespace strips {
Object::Object() : Reference() {}
Object::Object(duk_context *ctx) : Reference(ctx) {}
Object::Object(duk_context *ctx, duk_idx_t idx) : Reference(ctx, idx) {}
Object::Object(const Object &o) : Reference(o) {}
Object::Object(Object &&o) : Reference(std::move(o)) {}
Object::~Object() {}

bool Object::has(const std::string &name) const {
  push();
  bool ret = duk_has_prop_string(ctx(), -1, name.c_str());
  duk_pop(ctx());
  return ret;
}

void Object::remove(const std::string &name) const {
  push();
  duk_del_prop_string(ctx(), -1, name.c_str());
  duk_pop(ctx());
}

bool Object::valid() const {
  if (!Reference::valid()) {
    return false;
  }
  push();

  bool ret = duk_is_object_coercible(ctx(), -1);
  duk_pop(ctx());

  return ret;
}

std::vector<std::string> Object::keys() const {
  duk_get_global_string(ctx(), "Object");
  duk_push_string(ctx(), "keys");
  push();
  std::vector<std::string> v;
  duk_ret_t ret = duk_pcall_prop(ctx(), -3, 1);
  if (ret != DUK_EXEC_SUCCESS) {
    duk_pop(ctx());
    return v;
  }
  duk_size_t len = duk_get_length(ctx(), -1);

  for (int i = 0; i < len; i++) {
    duk_get_prop_index(ctx(), -1, i);
    const char *k = duk_get_string(ctx(), -1);
    duk_pop(ctx());
    v.push_back(k);
  }
  duk_pop_2(ctx());
  return std::move(v);
}

size_t Object::size() const { return keys().size(); }

// oid Object::push() const { duk_push_ref(ctx(), d->ref); }

/*Object Object::clone() const {
  duk_get_global_string(ctx(), "Object");
  duk_push_string(ctx(), "create");
  duk_push_ref(ctx(), d->ref);
  duk_pcall_prop(ctx(), -3, 1);
  Object ob(ctx(), duk_ref(ctx()));
  duk_pop(ctx());
  return std::move(ob);
}*/

std::ostream &operator<<(std::ostream &o, const Object &v) {
  v.push();
  duk_json_encode(v.ctx(), -1);
  const char *str = duk_get_string(v.ctx(), -1);
  duk_pop(v.ctx());
  o << std::string(str);
  return o;
}

} // namespace strips
