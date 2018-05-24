#include <string>
#include <strips++/array.hpp>

namespace strips {

/*std::ostream &operator<<(std::ostream &o, const Array &v) {
  v.push();
  duk_json_encode(v.ctx(), -1);
  const char *str = duk_get_string(v.ctx(), -1);
  duk_pop(v.ctx());
  o << std::string(str);
  return o;
}*/
} // namespace strips