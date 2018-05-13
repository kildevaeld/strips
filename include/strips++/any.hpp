#pragma once
#include <type_traits>
#include <utility>
#include <typeinfo>
#include <string>
#include <cassert>
#include <duktape.h>

namespace strips {


/*template <typename T>
class has_to_duktape
{
  struct no {};

  template <typename T2>
  static decltype(std::declval<duk_context *>() << std::declval<T2>()) test(int);

  template <typename T2>
  static no test(...);

public:
  enum { value = ! std::is_same<no, decltype(test<T>(0))>::value};
};*/

template<class T>
using StorageType = typename std::decay<typename std::remove_reference<T>::type>::type;

struct Any
{
    bool is_null() const { return !ptr; }
    bool not_null() const { return ptr; }

    template<typename U> Any(U&& value)
        : ptr(new Derived<StorageType<U>>(std::forward<U>(value)))
    {

    }

    template<class U> bool is() const
    {
        typedef StorageType<U> T;

        auto derived = dynamic_cast<Derived<T>*> (ptr);

        return derived;
    }

    template<class U>
    StorageType<U>& as()
    {
        typedef StorageType<U> T;

        auto derived = dynamic_cast<Derived<T>*> (ptr);

        if (!derived)
            throw std::bad_cast();

        return derived->value;
    }

    void push_duktape(duk_context *ctx) const {
        ptr->push_to_duktape(ctx);
    }

    template<class U>
    operator U()
    {
        return as<StorageType<U>>();
    }

    Any()
        : ptr(nullptr)
    {

    }

    Any(Any& that)
        : ptr(that.clone())
    {

    }

    Any(Any&& that)
        : ptr(that.ptr)
    {
        that.ptr = nullptr;
    }

    Any(const Any& that)
        : ptr(that.clone())
    {

    }

    Any(const Any&& that)
        : ptr(that.clone())
    {

    }

    Any& operator=(const Any& a)
    {
        if (ptr == a.ptr)
            return *this;

        auto old_ptr = ptr;

        ptr = a.clone();

        if (old_ptr)
            delete old_ptr;

        return *this;
    }

    Any& operator=(Any&& a)
    {
        if (ptr == a.ptr)
            return *this;

        std::swap(ptr, a.ptr);

        return *this;
    }

    ~Any()
    {
        if (ptr)
            delete ptr;
    }

private:
    struct Base
    {
        virtual ~Base() {}

        virtual Base* clone() const = 0;
        virtual void push_to_duktape(duk_context *ctx) const = 0;
    };

    template<typename T>
    struct Derived : Base
    {
        template<typename U> Derived(U&& value) : value(std::forward<U>(value)) { }

        T value;

        Base* clone() const { return new Derived<T>(value); }
        void push_to_duktape(duk_context *ctx) const {
            to_duktape(ctx, value);
        }
    };

    Base* clone() const
    {
        if (ptr)
            return ptr->clone();
        else
            return nullptr;
    }

    Base* ptr;
};

}