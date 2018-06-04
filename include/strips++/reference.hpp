#pragma once
#include <duktape.h>
#include <memory>
#include <strips/utils.h>

namespace strips {

namespace internal {
class ReferencePrivate;
}

enum class Type {
  String,
  Number,
  Bool,
  Object,
  Array,
  Null,
  Undefined,
  Function,
  Buffer,
  Date
};

std::ostream &operator<<(std::ostream &s, const Type &);

class Reference {

public:
  Reference();
  Reference(duk_context *ctx);
  Reference(duk_context *ctx, duk_idx_t idx);
  Reference(const Reference &);
  Reference(Reference &&);

  Reference &operator=(const Reference &);
  Reference &operator=(Reference &&);

  virtual ~Reference();

  template <class T> T as() const {
    push();
    T v;
    from_duktape(ctx(), -1, v);
    duk_pop(ctx());
    return std::move(v);
  }

  Reference ref_up() const {
    Reference r;
    r.set_ctx(ctx());
    push();
    r.set_ref(duk_ref(ctx()));
    return std::move(r);
  }

  duk_context *ctx() const;

  virtual bool valid() const;
  Type type() const;
  void push() const;

  template <Type tmp> bool is() const { return type() == tmp; }

  friend std::ostream &operator<<(std::ostream &s, const Reference &);

  template <typename T> static Reference create(duk_context *ctx, const T &v) {
    to_duktape(ctx, v);
    Reference ref(ctx);
    duk_pop(ctx);
    return std::move(ref);
  }

  void unref();

protected:
  int ref() const;
  void set_ref(int ref);
  void set_ctx(duk_context *ctx);

private:
  friend void from_duktape(duk_context *ctx, duk_idx_t idx, Reference &o);
  friend class Object;
  std::unique_ptr<internal::ReferencePrivate> ptr;
};

} // namespace strips