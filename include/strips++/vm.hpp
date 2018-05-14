#pragma once
#include <memory>
#include <strips++/any.hpp>
#include <strips++/array.hpp>
#include <strips++/converters.hpp>
#include <strips++/function.hpp>
#include <strips++/object.hpp>
//#include <strips++/builder.hpp>

namespace strips {

namespace internal {
class VMPrivate;
}

class VM {

public:
  VM(duk_context *ctx = NULL);
  VM(const VM &) = delete;
  ~VM();

  Reference eval_path(const std::string &) ;
  Reference eval_script(const std::string &, const std::string &) ;
  Reference eval(const std::string &) ;

  duk_context *ctx() const;

  Object object(const std::map<std::string, Any> &list) {
    push(list);
    auto o = pop<Object>();
    return std::move(o);
  }

  Object object() const;
  template <typename... Args> Array array(Args... args) {
    duk_push_array(ctx());
    int size = 0;
    iterate_vaargs(
        [&](auto &arg) {
          to_duktape(ctx(), arg);
          duk_put_prop_index(ctx(), -2, size++);
        },
        args...);
    return std::move(pop<Array>());
  }
  Array array() const;
  Function ctor(const Object &o) const;

  template <class T> Reference create(const T &v) {
    to_duktape(ctx(), v);
    Reference ref(ctx());
    duk_pop(ctx());
    return std::move(ctx());
  }

  /*template <typename T>ClassBuilder ctor(T fn) {
    ClassBuilder builder(ctx(), std::move(fn));
    return std::move(builder);
  }*/

  template <class T> VM &push(const T &v) {
    to_duktape(ctx(), v);
    return *this;
  }

  template <class T> VM &push(T &v) {
    to_duktape(ctx(), v);
    return *this;
  }

  /*emplate <class T> const VM &push(T &v) {
    to_duktape(ctx(), v);
    return *this;
  }*/

  template <class T = Reference> T get(duk_idx_t idx = -1) const {
    T v;
    from_duktape(ctx(), idx, v);
    return std::move(v);
  }

  template <class T> T pop()  {
    T v = get<T>(-1);
    duk_pop(ctx());
    return std::move(v);
  }

  template <class T = Object> T current_this() const {
    duk_push_this(ctx());
    T v = get<T>(-1);
    duk_pop(ctx());
    return std::move(v);
  }

  Function current_function() const {
    duk_push_current_function(ctx());
    Function f(ctx(), -1);
    duk_pop(ctx());
    return std::move(f);
  }

  Object global() const;
  Object stash() const;
  Object require(const std::string &name) const;

  const VM &dump() const;

  duk_size_t top() const;
  const VM &pop(int count = 1) ;
  const VM &remove(duk_idx_t idx) ;

private:
  std::unique_ptr<internal::VMPrivate> d;
};

namespace details {

template<>
class Callable<std::function<duk_ret_t(VM &vm)>>: public ::strips::Callable {

public:
    Callable(std::function<duk_ret_t(VM &vm)> &&fn): m_fn(std::move(fn)) {}
    duk_ret_t call(duk_context *ctx) const override {
        VM vm(ctx);
        return m_fn(vm);
    }

    duk_ret_t call(duk_context *ctx) override {
        VM vm(ctx);
        return m_fn(vm);
    }


private:
    std::function<duk_ret_t(VM &vm)> m_fn;
};

}

} // namespace strips