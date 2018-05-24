#pragma once
#include <strips++/reference.hpp>

namespace strips {

class Array : public Reference {

public:
  class iterator {
  public:
    typedef iterator self_type;
    typedef Reference value_type;
    typedef Reference &reference;
    typedef Reference *pointer;
    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    iterator(Array *ptr) : m_obj(ptr), m_current() {
      if (ptr == nullptr) {
        m_idx = -1;
      } else {
        m_size = m_obj->size();
      }

      if (m_size > 0) {
        m_current = std::move(m_obj->index(0));
      }
    }
    self_type operator++() {
      self_type i = *this;
      m_idx++;
      if (m_idx > m_size) {
        throw std::runtime_error("overflow");
      } else if (m_idx == m_size) {
        m_idx = -1;
        return *this;
      }

      m_current = m_obj->index(m_size);
      return i;
    }
    self_type operator++(int junk) {

      m_idx++;
      if (m_idx > m_size) {
        throw std::runtime_error("overflow");
      } else if (m_idx == m_size) {
        m_idx = -1;
        return *this;
      }

      m_current = std::move(m_obj->index(m_idx));
      return *this;
    }
    reference operator*() { return m_current; }
    pointer operator->() { return &m_current; }
    bool operator==(const self_type &rhs) { return m_idx == rhs.m_idx; }
    bool operator!=(const self_type &rhs) { return m_idx != rhs.m_idx; }

  private:
    size_t m_idx = 0;
    Array *m_obj = nullptr;
    size_t m_size;
    Reference m_current;
  };

  Array() : Reference() {}
  Array(duk_context *ctx) : Reference(ctx) {}
  Array(duk_context *ctx, duk_idx_t idx) : Reference(ctx, idx) {}
  Array(const Array &o) : Reference(o) {}
  Array(Array &&o) : Reference(std::move(o)) {}
  virtual ~Array() {}

  template <class T> void push_back(const T &v) {
    push();
    try {
      to_duktape(ctx(), v);
    } catch (...) {
      duk_pop(ctx());
      return;
    }
    duk_put_prop_index(ctx(), -2, size());
    duk_pop(ctx());
  }

  template <class T = Reference> T index(size_t idx) {
    if (idx > size()) {
      throw std::runtime_error("overflow");
    }
    push();
    duk_get_prop_index(ctx(), -1, idx);
    T v;
    try {
      from_duktape(ctx(), -1, v);
    } catch (...) {
      duk_pop(ctx());
      return std::move(v);
    }

    duk_pop_2(ctx());
    return std::move(v);
  }

  size_t size() const {
    push();
    auto len = duk_get_length(ctx(), -1);
    duk_pop(ctx());
    return len;
  }

  iterator begin() { return iterator(this); }
  iterator end() { return iterator(nullptr); }

  bool valid() const override { return type() == Type::Array; }

  // friend std::ostream &operator<<(std::ostream &s, const Array &);
};

} // namespace strips