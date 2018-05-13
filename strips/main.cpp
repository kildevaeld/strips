#include <iostream>
#include <strips++/value.hpp>
#include <strips++/vm.hpp>
#include <strips/io/io.h>

using namespace strips;
namespace tem {

struct Data {
  std::string name;
  std::string content;
};

void to_duktape(const VM &ctx, const Data &data) {
  /*duk_push_object(ctx);
  duk*/
  auto o = ctx.object();
  o.set("name", data.name);
  o.set("content", data.content);
  o.push();
}

void from_duktape(const VM &ctx, duk_idx_t idx, Data &data) {
  auto o = ctx.get<Object>(idx);
  data.name = o.get<std::string>("name");
  data.content = o.get<std::string>("content");
}

} // namespace tem

int main() {

  VM vm;
  strips_io_init(vm.ctx());

  auto a = vm.array("Test", "mig", 2330.2, false, 122);

  auto io = vm.require("io");

  auto file = io.get<Function>("File").construct("../main.js", "r");
  auto ret = file.call<Object>("read", 0, 30303);

  std::cout << ret.get<int>("length") << std::endl;
}