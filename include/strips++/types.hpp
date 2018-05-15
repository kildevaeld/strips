#pragma once
#include <duktape.h>
#include <functional>
#include <map>
#include <string>
#include <strips++/any.hpp>

namespace strips {

using Callback = std::function<duk_ret_t(duk_context *ctx)>;

using ObjectLiteral = std::map<std::string, Any>;
} // namespace strips