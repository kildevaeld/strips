#include <iostream>
#include <strips++/value.hpp>
#include <strips++/vm.hpp>
#include <strips/curl/curl.h>
#include <strips/exec/exec.h>
#include <strips/io/io.h>
#include <strips/os/os.h>
#include <strips/path/path.h>
#include <strips/prompt/prompt.h>
#include <syrup/fs.h>
#include <syrup/path.h>

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
  std::cout << "usage: zap <path>" << std::endl;
  return ret;
}


static void init_vm(VM &vm, int argc, char **argv) {
  strips_path_init(vm.ctx());
  strips_prompt_init(vm.ctx());
  strips_io_init(vm.ctx());
  strips_curl_init(vm.ctx());
  strips_exec_init(vm.ctx());
  strips_os_init(vm.ctx(), argc, argv, NULL);
}


int main(int argc, char *argv[]) {

  std::vector<std::string> args(argv + 1, argv + argc);


  if (argc < 2) {
    return print_help(1);
  } else if (args[0] == "-h" || args[0] == "--help") {
    return print_help();
  } /*else if (args[0][0] == '-') {
    return print_help(1);
  }*/ else if (args[1] == "--version" ||args[1] == "-v" || args[1] == "version") {
    std::cout << "strips 0.0.1" << std::endl;
    return 0; 
  }

  VM vm;
  init_vm(vm, argc,argv);

  char path[PATH_MAX];

  if (!sy_file_exists(argv[1])) {
    size_t idx, len;
    len = sy_path_ext(argv[1], &idx);
    if (len != 0) {
      std::cerr << "file does not exists: " << argv[1] << std::endl;
      return 5;
    }
    len = strlen(argv[1]);

    memcpy(path, argv[1], len);
    memcpy(path + len, ".js", 3);
    path[len + 3] = '\0';

    if (!sy_file_exists(path)) {
      std::cerr << "file does not exists: " << path << std::endl;
      return 5;
    }
  } else {
    if (sy_file_is_regfile(argv[1])) {
      strcpy(path, argv[1]);
    } else if (sy_file_is_dir(argv[1])) {

      if (!sy_path_join(path, argv[1], "index.js", NULL)) {
        std::cerr << "could not resolve path" << std::endl;
      }
      if (!sy_file_exists(path)) {
        std::cerr << "file does not exists: " << path << std::endl;
        return 5;
      }
    }
  }

  try {
    auto result = vm.eval_path(path);
  } catch (const std::runtime_error &e) {
    std::cerr << "could execute javascript: " << e.what() << std::endl;
    return 4;
  }

  /*if (!result.is<Type::Invalid>()) {
    std::cout << result << s  td::endl;
  }*/

  return 0;
}