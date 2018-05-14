#include <csystem/csystem++.hpp>
#include <iostream>
#include <strips++/value.hpp>
#include <strips++/vm.hpp>
#include <strips/curl/curl.h>
#include <strips/exec/exec.h>
#include <strips/io/io.h>
#include <strips/os/os.h>

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

int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cerr << "usage: strips2 <path>" << std::endl;
    return 1;
  }

  VM vm;
  strips_io_init(vm.ctx());
  strips_curl_init(vm.ctx());
  strips_exec_init(vm.ctx());
  strips_os_init(vm.ctx(), argc, argv, NULL);

  std::vector<std::string> args(argv + 1, argv + argc);
  args[0] = csystem::path::join(csystem::standardpaths::cwd(), args[0]);

  auto process = vm.object({{"argv", args},
                            {"platform", cs_platform_name()},
                            {"cwd", [](VM &vm) {
                               vm.push(csystem::standardpaths::cwd());
                               return 1;
                             }}});

  vm.global().set("process", process);

  
  auto ref = vm.push([](const VM &vm) {
    vm.current_this().set("what", vm.get(0));
    return 0;
  }).pop<Function>();

  auto proto = vm.object({
    {"fn", [](VM &vm){
      vm.current_this().get("what").push();
      return 1;
    }}
  });

  ref.set("prototype", proto);
  
  vm.global().set("Test", ref);
  

  auto result = vm.eval_path(argv[1]);

  /*if (!result.is<Type::Invalid>()) {
    std::cout << result << s  td::endl;
  }*/

  return 0;
}