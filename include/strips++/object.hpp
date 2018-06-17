#pragma once
#include <duktape.h>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <strips++/reference.hpp>
#include <strips++/types.hpp>
#include <strips++/utils.hpp>
#include <strips/utils.h>
#include <vector>

namespace strips {

class VM;

class Object : public Reference {

public:
  class iterator {
  public:
    typedef iterator self_type;
    typedef std::pair<std::string, Reference> value_type;
    typedef std::pair<std::string, Reference> &reference;
    typedef std::pair<std::string, Reference> *pointer;
    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    iterator(Object *ptr) : m_obj(ptr), m_current({}) {
      if (ptr == nullptr) {
        m_idx = -1;
      } else {
        m_keys = m_obj->keys();
      }

      if (m_keys.size() > 0) {
        m_current.first = m_keys[0];
        m_current.second = m_obj->get(m_keys[0]);
      }
    }
    self_type operator++() {
      self_type i = *this;
      m_idx++;
      if (m_idx > m_keys.size()) {
        throw std::runtime_error("overflow");
      } else if (m_idx == m_keys.size()) {
        m_idx = -1;
        return *this;
      }
      auto &key = m_keys[m_idx];
      m_current.first = std::string(key);
      m_current.second = m_obj->get(key);
      return i;
    }
    self_type operator++(int junk) {

      m_idx++;
      if (m_idx > m_keys.size()) {
        throw std::runtime_error("overflow");
      } else if (m_idx == m_keys.size()) {
        m_idx = -1;
        return *this;
      }
      auto &key = m_keys[m_idx];
      m_current.first = std::string(key);
      m_current.second = m_obj->get(key);
      return *this;
    }
    reference operator*() { return m_current; }
    pointer operator->() { return &m_current; }
    bool operator==(const self_type &rhs) { return m_idx == rhs.m_idx; }
    bool operator!=(const self_type &rhs) { return m_idx != rhs.m_idx; }

  private:
    size_t m_idx = 0;
    Object *m_obj = nullptr;
    std::vector<std::string> m_keys;
    std::pair<std::string, Reference> m_current;
  };

  Object(duk_context *ctx);
  Object(duk_context *ctx, duk_idx_t idx);
  Object(const Object &);
  Object(Object &&);
  virtual ~Object();

  template <class T> void set(const std::string &name, const T &v) const {
    push();
    to_duktape(ctx(), v);
    duk_put_prop_string(ctx(), -2, name.c_str());
    duk_pop(ctx());
  }

  template <class T = Reference> T get(const std::string &name) const {
    push();
    T v;
    if (!duk_has_prop_string(ctx(), -1, name.c_str())) {
      throw std::runtime_error("no name " + name);
    }

    duk_get_prop_string(ctx(), -1, name.c_str());
    from_duktape(ctx(), -1, v);
    duk_pop_2(ctx());
    return std::move(v);
  }

  template <class T = Reference> T del(const std::string &name) const {
    push();
    duk_del_prop_string(ctx(), -1, name.c_str());
    duk_pop(ctx());
  }

  /*void set_finalizer(Callback cb) {
    push();
    to_duktape(ctx(), cb);
  }*/

  template <typename T = Reference, typename... Args>
  T call(const std::string &prop, const Args &... args) {

    push();
    duk_idx_t idx = duk_normalize_index(ctx(), -1);
    duk_push_string(ctx(), prop.c_str());
    int size = 0;
    iterate_vaargs(
        [&](auto &arg) {
          to_duktape(ctx(), arg);
          size++;
        },
        args...);

    duk_ret_t ret = duk_pcall_prop(ctx(), idx, size);
    duk_remove(ctx(), -2);

    if (ret != DUK_EXEC_SUCCESS) {
      const char *msg = NULL;
      if (duk_has_prop_string(ctx(), -1, "stack")) {
        duk_get_prop_string(ctx(), -1, "stack");
        msg = duk_get_string(ctx(), -1);
        duk_pop(ctx());
      }
      throw std::runtime_error(msg);
    }

    T v;
    from_duktape(ctx(), -1, v);

    duk_pop(ctx());

    return std::move(v);
  }

  bool has(const std::string &name) const;

  void remove(const std::string &name) const;

  std::vector<std::string> keys() const;
  bool valid() const override;

  void set_finalizer(std::function<duk_ret_t(VM &vm)> fn);

  size_t size() const;
  iterator begin() { return iterator(this); }
  iterator end() { return iterator(nullptr); }

  friend std::ostream &operator<<(std::ostream &s, const Object &);

private:
  friend class VM;
  friend class Reference;
  friend class Value;
  friend class Function;
  friend void from_duktape(duk_context *ctx, duk_idx_t idx, Object &o);
  Object();

}; // namespace strips

} // namespace strips