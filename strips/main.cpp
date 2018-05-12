#include <iostream>
#include <strips++/strips++.hpp>
#include <strips++/value.hpp>

using namespace strips;
int main() {

  VM vm;

  vm.global().get<Object>("Object").get("create").push();

  auto o = vm.object();

  o.set("rapp", "rpper");

  Object clone = o.clone();
  clone.push();

  vm.push(o);

  vm.dump();

  Object ob = vm.get<Object>();
  ob.set("test", "rapp");
  Value v(vm.ctx(), "Rapper");

  o.set("fn", [](VM &ctx) {
    std::cout << "raprap: " << ctx.get<std::string>(0) << std::endl;
    return 0;
  });

  o.call("fn", "Hello, Qorld!");

  std::cout << v.as<std::string>() << "\n";

  std::cout << ob << std::endl;

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