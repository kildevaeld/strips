#pragma once
#include <duktape.h>
#include <functional>
#include <map>
#include <string>
#include <strips/utils.h>
#include <vector>

namespace strips {

class VM;
class Object;
class Value;

void to_duktape(duk_context *ctx, std::function<duk_ret_t(VM &)> fn);

void to_duktape(duk_context *ctx, duk_c_function fn);

void to_duktape(duk_context *ctx, const std::string &str);

void from_duktape(duk_context *ctx, duk_idx_t idx, std::string &str);

/*void to_duktape(duk_context *ctx, const bool &str);

void from_duktape(duk_context *ctx, duk_idx_t idx, bool &str);*/

template <class T,
          typename std::enable_if<std::is_integral<T>::value>::type * = nullptr>
void to_duktape(duk_context *ctx, const T &v) {
  duk_push_int(ctx, (int)v);
}

template <class T,
          typename std::enable_if<std::is_integral<T>::value>::type * = nullptr>
void from_duktape(duk_context *ctx, duk_idx_t idx, T &i) {
  duk_double_t d = duk_get_number(ctx, idx);
  i = (int)d;
}

template <class T, typename std::enable_if<
                       std::is_floating_point<T>::value>::type * = nullptr>
void to_duktape(duk_context *ctx, const T &v) {
  duk_push_number(ctx, (duk_double_t)v);
}

template <class T, typename std::enable_if<
                       std::is_floating_point<T>::value>::type * = nullptr>
void from_duktape(duk_context *ctx, duk_idx_t idx, T &i) {
  duk_double_t d = duk_get_number(ctx, idx);
  i = d;
}

template <class T> void to_duktape(duk_context *ctx, const std::vector<T> &v) {
  duk_push_array(ctx);
  for (size_t i = 0; i < v.size(); i++) {
    to_duktape(ctx, v[i]);
    duk_put_prop_index(ctx, -2, i);
  }
}

template <class T>
void from_duktape(duk_context *ctx, duk_idx_t idx, std::vector<T> &v) {
  if (!duk_is_array(ctx, idx)) {
    return;
  }

  duk_idx_t aidx = duk_normalize_index(ctx, idx);

  int len = duk_get_length(ctx, aidx);

  for (int i = 0; i < len; i++) {
    T a;
    duk_get_prop_index(ctx, aidx, i);
    from_duktape(ctx, -1, a);
    duk_pop(ctx);
    v.push_back(a);
  }
}

template <class T>
void to_duktape(duk_context *ctx, const std::map<std::string, T> &v) {
  duk_push_object(ctx);
  for (auto &a : v) {
    to_duktape(ctx, a.second);
    duk_put_prop_string(ctx, -2, a.first.c_str());
  }
}

template <class T>
void from_duktape(duk_context *ctx, duk_idx_t idx,
                  std::map<std::string, T> &v) {
  if (!duk_is_object(ctx, idx)) {
    return;
  }

  duk_idx_t aidx = duk_normalize_index(ctx, idx);

  duk_enum(ctx, aidx, DUK_ENUM_OWN_PROPERTIES_ONLY);
  while (duk_next(ctx, -1, 1)) {
    const char *key = duk_get_string(ctx, -2);
    T a;
    from_duktape(ctx, -1, a);
    v[key] = std::move(a);
    duk_pop_2(ctx);
  }
  duk_pop(ctx);
}

void to_duktape(duk_context *ctx, const Object &o);

void from_duktape(duk_context *ctx, duk_idx_t idx, Object &o);

void to_duktape(duk_context *ctx, const Value &o);

void from_duktape(duk_context *ctx, duk_idx_t idx, Value &o);

} // namespace strips