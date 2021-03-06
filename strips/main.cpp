#include <iostream>
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

static int print_help(int ret = 0) {
  std::cout << "usage: zap <path>" << std::endl;
  return ret;
}

class Mo : public ModuleResolver {

public:
  std::string name() const { return "weed"; }
  virtual std::vector<std::string> resolve(const std::string &id,
                                           const std::string &parent) const {
    std::vector<std::string> list;
    if (id == "testmig") {
      list.push_back("hello.js");
    } else if (id == "text.txt") {
      list.push_back("text.txt");
    }

    return list;
  }
  virtual ModuleResolverLoadResult
  load(const std::vector<std::string> &files) const {

    ModuleResolverLoadResult result;

    for (auto a : files) {
      if (a == "text.txt")
        result.emplace_back(a, "Hello, World");
      else
        result.emplace_back(a, "exports.test = 'Hello world'");
    }

    return std::move(result);
  }
};

class MP : public ModuleParser {

public:
  MP() {}
  virtual std::string extension() const { return ".txt"; }
  virtual void parse(const Object &module, const std::string &content) const {
    module.set("exports", content);
  }
};

class Factory {

public:
  duk_ret_t operator()(VM &vm) {
    auto fn = vm.push([](VM &vm) {
                  vm.get_this().set_finalizer([](VM &vm) {
                    std::cout << "quit" << std::endl;
                    return 0;
                  });
                  return 0;
                })
                  .pop<Function>();

    fn.prototype({{"name", "cpp"}, {"call", [](VM &vm) { return 0; }}});

    vm.object({{"Test", fn}}).push();

    return 1;
  }
};

static void init_vm(VM &vm, int argc, char **argv) {
  strips_path_init(vm.ctx());
  strips_prompt_init(vm.ctx());
  strips_io_init(vm.ctx());
  strips_curl_init(vm.ctx());
  strips_exec_init(vm.ctx());
  strips_os_init(vm.ctx(), argc, argv, NULL);

  vm.register_module("cpp", Factory());
  vm.set_module_resolver(new Mo());
  vm.set_module_parser(new MP());
}

int main(int argc, char *argv[]) {

  std::vector<std::string> args(argv + 1, argv + argc);

  if (argc < 2) {
    return print_help(1);
  } else if (args[0] == "-h" || args[0] == "--help") {
    return print_help();
  } else if (args[0] == "--version" || args[0] == "-v" ||
             args[0] == "version") {
    std::cout << "zap 0.0.2" << std::endl;
    return 0;
  }

  VM vm;
  init_vm(vm, argc, argv);

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

  // vm.stash().get("strips").push();
  // vm.dump();

  return 0;
}