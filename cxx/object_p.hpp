#pragma once

namespace strips {
    namespace internal {

class ObjectPrivate {

public:
  ObjectPrivate(duk_context *c, int r) : ctx(c), ref(r) {}
  ~ObjectPrivate() { duk_unref(ctx, ref); }
  duk_context *ctx;
  int ref;
  friend void from_duktape(duk_context *ctx, duk_idx_t idx, Object &o);
};

} // namespace internal
}