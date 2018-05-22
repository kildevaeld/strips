#include <iostream>
#include <strips++/value.hpp>
#include <strips++/vm.hpp>
#include <strips/curl/curl.h>
#include <strips/exec/exec.h>
#include <strips/io/io.h>
#include <strips/os/os.h>
#include <strips/path/path.h>
#include <strips/prompt/prompt.h>

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

static int print_help(int ret = 0) {
  std::cout << "usage: strips2 <path>" << std::endl;
  return ret;
}

int main(int argc, char *argv[]) {

  std::vector<std::string> args(argv + 1, argv + argc);

  if (argc < 2) {
    return print_help(1);
  } else if (args[0] == "-h" || args[0] == "--help") {
    return print_help();
  } else if (args[0][0] == '-') {
    return print_help(1);
  }

  VM vm;
  strips_path_init(vm.ctx());
  strips_prompt_init(vm.ctx());
  strips_io_init(vm.ctx());
  strips_curl_init(vm.ctx());
  strips_exec_init(vm.ctx());
  strips_os_init(vm.ctx(), argc, argv, NULL);

  /*args[0] = csystem::path::join(csystem::standardpaths::cwd(), args[0]);

  auto process = vm.object({{"argv", args},
                            {"platform", cs_platform_name()},
                            {"cwd", [](VM &vm) {
                               vm.push(csystem::standardpaths::cwd());
                               return 1;
                             }}});

  vm.global().set("process", process);

  std::string fname = argv[1];
  int idx;
  if (!cs_file_exists(argv[1]) && cs_path_ext(argv[1], &idx) == 0) {
    fname = fname + ".js";
    if (!cs_file_exists(fname.c_str())) {
      std::cerr << "File not found" << std::endl;
      return 3;
    }
  } else {
    std::cerr << "File not found " << cs_path_ext(argv[1], &idx) << std::endl;
    return 3;
  }*/

  try {
    auto result = vm.eval_path(argv[1]);
  } catch (const std::runtime_error &e) {
    std::cerr << "could execute javascript: " << e.what() << std::endl;
    return 4;
  }

  /*if (!result.is<Type::Invalid>()) {
    std::cout << result << s  td::endl;
  }*/

  return 0;
}