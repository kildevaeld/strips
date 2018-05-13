#include <iostream>
#include <strips++/value.hpp>
#include <strips++/vm.hpp>
#include <strips/curl/curl.h>
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
  strips_curl_init(vm.ctx());

  auto a = vm.array("Test", "mig", 2330.2, false, 122);

  auto curl = vm.require("curl");

  vm.object({{
    "Hello", true
  }});

  /*auto o = vm.object({
    {"url", "wotld"}, 
    {"rapper", "raprap"},
    {"raprap", a.ref_up()}
  });
  o.set("url", "https://google.com");
  std::cout << o << std::endl;*/

  auto o = vm.object({
    {"url", "https://google.com"}
  });
  
  auto request = curl.get<Function>("Request").construct(o);

  std::cout << request << std::endl;
  // auto resp = curl.call<Object>("req", request);
  // std::cout << resp.get("header") << std::endl;
  // std::cout << resp.get<int>("length") << std::endl;
  /*curl.unref();
  o.unref();
  vm.stash().push();
  vm.dump().pop().dump();*/
}