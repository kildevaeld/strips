#include <strips++/converters.hpp>
#include <strips++/module-resolver.hpp>
#include <strips++/object.hpp>
#include <strips++/vm.hpp>
#include <strips/modules.h>

namespace strips {

void ModuleParser::push(duk_context *ctx) {

  strips_set_module_parser(ctx, extension().c_str(), ModuleParser::native_call);

  duk_push_global_stash(ctx);
  Object obj(ctx, -1);
  duk_pop(ctx);

  auto resolvers =
      obj.get<Object>("strips").get<Object>("parsers").get<Function>(
          extension().c_str());

  resolvers.set_ptr("ptr", this);
  resolvers.push();
  duk_push_c_lightfunc(ctx, ModuleParser::native_finalizer, 1, 1, 0);
  duk_set_finalizer(ctx, -2);

  duk_pop(ctx);
}

duk_ret_t ModuleParser::native_call(duk_context *ctx) {

  Object module(ctx, 0);
  const char *content = duk_get_string(ctx, 1);

  duk_push_current_function(ctx);
  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("ptr"));
  auto parser = reinterpret_cast<ModuleParser *>(duk_get_pointer(ctx, -1));
  duk_pop_2(ctx);

  parser->parse(module, content);

  return 0;
}

duk_ret_t ModuleParser::native_finalizer(duk_context *ctx) {
  if (!duk_has_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("ptr"))) {
    return 0;
  }
  duk_get_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("ptr"));
  auto ptr = reinterpret_cast<ModuleParser *>(duk_get_pointer(ctx, -1));
  delete ptr;
  return 0;
}

void ModuleResolver::push(duk_context *ctx) {
  strips_set_module_resolver(ctx, name().c_str(),
                             ModuleResolver::native_resolve,
                             ModuleResolver::native_load);

  duk_push_global_stash(ctx);
  Object obj(ctx, -1);
  duk_pop(ctx);

  auto resolvers = obj.get<Object>("strips")
                       .get<Object>("resolvers")
                       .get<Object>(name().c_str());

  resolvers.set_ptr("ptr", this);
  resolvers.push();
  duk_push_c_lightfunc(ctx, ModuleResolver::native_finalizer, 1, 1, 0);
  duk_set_finalizer(ctx, -2);

  duk_pop(ctx);
}

duk_ret_t ModuleResolver::native_resolve(duk_context *ctx) {
  duk_push_global_stash(ctx);
  Object stash(ctx, -1);
  duk_pop(ctx);

  Object module(ctx, 0);

  auto protocol = module.get<Array>("protocol").get<std::string>(1);
  auto resolver = stash.get<Object>("strips")
                      .get<Object>("resolvers")
                      .get<Object>(protocol.c_str());

  auto ptr = reinterpret_cast<ModuleResolver *>(resolver.get_ptr("ptr"));

  auto id = module.get<std::string>("id");
  id = id.substr(protocol.size() + 3);
  auto files = ptr->resolve(id, module.get<std::string>("parent"));

  module.set("files", files);

  return 0;
}
duk_ret_t ModuleResolver::native_load(duk_context *ctx) {

  Object builder(ctx, 0);
  Object module(ctx, 1);

  auto ptr = reinterpret_cast<ModuleResolver *>(
      builder.get<Object>("resolver").get_ptr("ptr"));

  auto files = builder.get<std::vector<std::string>>("files");

  ModuleResolverLoadResult results;
  try {
    results = ptr->load(files);
  } catch (...) {
    duk_type_error(ctx, "could not load file");
  }

  if (results.size() != files.size()) {
    duk_type_error(ctx, "could not load file");
  }

  auto output = builder.get<Array>("files");

  for (int i = 0, len = output.size(); i < len; i++) {
    duk_push_object(ctx);
    Object o(ctx, -1);
    o.set("file", results[i].file);
    o.set("content", results[i].content);
    output.set(i, o);
  }
  return 0;
}

duk_ret_t ModuleResolver::native_finalizer(duk_context *ctx) {
  if (!duk_has_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("ptr"))) {
    return 0;
  }
  duk_get_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("ptr"));
  auto ptr = reinterpret_cast<ModuleResolver *>(duk_get_pointer(ctx, -1));
  delete ptr;
  return 0;
}

} // namespace strips