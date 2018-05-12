#include <iostream>
#include <strips++/value.hpp>
#include <strips++/vm.hpp>
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

  auto g = vm.global();
  auto o = vm.object();
  if (o.is<Type::Object>()) {
  }
  tem::Data data;
  data.name = "Test";
  data.content = "Content";
  auto fn = vm.push([](VM &vm) {
                vm.push("Test mig");
                return 1;
              })
                .pop<Function>();

  std::cout << fn.construct() << std::endl;

  o.set("constructor", [](auto &ctx) {
    std::cout << "constructor" << std::endl;
    return 0;
  });

  auto a = vm.array();

  auto cl = vm.ctor(o);

  a.push_back("Hello, World");

  for (auto i : a) {
    std::cout << "array " << i << std::endl;
  }

  o.set("fn", [](VM &ctx) {
    std::cout << "raprap: " << ctx.get<std::string>(0) << std::endl;
    auto o = ctx.object();
    o.set("hval", "borg");
    o.set("shit", 1220.1221);
    o.push();
    ctx.dump();
    return 1;
  });

  auto ret = o.call<Object>("fn", "Hello, Woeld");
  std::cout << "return " << ret;
  /*o.set("rapp", "rpper");
  o.set("test", "test");*/

  std::cout << o << std::endl;
  // cl.push();
  cl.construct();
  vm.dump();
  /*
    Object ob = vm.get<Object>();
    ob.set("test", "rapp");

    o.set("fn", [](VM &ctx) {
      std::cout << "raprap: " << ctx.get<std::string>(0) << std::endl;
      return 0;
    });

    o.call("fn", "Hello, Qorld!");


    std::cout << ob << std::endl;*/

  /*

  auto o = vm.object();
  o.set("Hello", "World");

  vm.push("Test mig");
  o.set("rap", 1000.39);
  o.set("Hello", true);
  o.set("fn", [](VM &vm) -> duk_ret_t {
    std::cout << "Herlig " << std::endl;
    vm.push("Hello, World");
    return 1;
  });

  std::cout << o;

  std::vector<std::string> list = {"Helo", "rapp"};
  o.set("rapraprap", list);
  // vm.dump();

  list = o.get<std::vector<std::string>>("rapraprap");
  for (auto l : list) {
    std::cout << l << "\n";
  }

  std::map<std::string, int> m = {{"Rapper", 202}, {"dsds", 2}};
  o.set("hoha", m);
  o.push();
  duk_get_prop_string(vm.ctx(), -1, "fn");
  Function fn(vm.ctx());

  fn.call("Hello", o);
  vm.dump();
  */
}