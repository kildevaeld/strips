#pragma once
#include <duktape.h>
#include <string>

namespace strips {

class Reference;

class ModuleParser {

public:
  virtual ~ModuleParser() {}

  virtual std::string extension() const = 0;
  virtual void parse(const Object &module,
                     const std::string &content) const = 0;

private:
  friend class VM;
  void push(duk_context *ctx);
  static duk_ret_t native_call(duk_context *ctx);
  static duk_ret_t native_finalizer(duk_context *ctx);
};

struct ModuleResolverLoadPair {

  ModuleResolverLoadPair(const std::string &n, std::string &&c)
      : file(n), content(std::move(c)) {}

  std::string file;
  std::string content;
};

using ModuleResolverLoadResult = std::vector<ModuleResolverLoadPair>;

class ModuleResolver {

public:
  virtual ~ModuleResolver() {}

  virtual std::string name() const = 0;
  virtual std::vector<std::string> resolve(const std::string &id,
                                           const std::string &parent) const = 0;
  virtual ModuleResolverLoadResult
  load(const std::vector<std::string> &file) const = 0;

private:
  friend class VM;
  void push(duk_context *ctx);
  static duk_ret_t native_resolve(duk_context *ctx);
  static duk_ret_t native_load(duk_context *ctx);
  static duk_ret_t native_finalizer(duk_context *ctx);
};

} // namespace strips