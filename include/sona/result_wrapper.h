#ifndef RESULT_WRAPPER_H
#define RESULT_WRAPPER_H

#include <memory>
#include <sona/pointer_plus.hpp>

namespace sona {

class result_wrapper_impl_base {
public:
  template <typename T> T const* get() const noexcept;
  template <typename T> T const& get_unsafe() const noexcept;
  virtual ~result_wrapper_impl_base() {}
};

template <typename T>
class result_wrapper_impl final : public result_wrapper_impl_base {
public:
  result_wrapper_impl(T const& value) : value(value) {}
  result_wrapper_impl(T &&value) : value(std::move(value)) {}

  T const& get_impl() const noexcept { return value; }

  ~result_wrapper_impl_base() noexcept {}

private:
  T value;
};

class result_wrapper {
public:
  template <typename T> result_wrapper(T const& value)
    : pbase(new result_wrapper_impl<T>(value)) {}

  template <typename T> result_wrapper(T&& value)
    : pbase(new result_wrapper_impl<T>(std::move(value))) {}

  template <typename T> T const* get() const noexcept {
    return pbase.borrow().get().get<T>();
  }

  template <typename T> T const& get_unsafe() const noexcept {
    return pbase.borrow().get().get_unsafe<T>();
  }

private:
  sona::owner<result_wrapper_impl_base> pbase;
};

template <typename T>
T const* result_wrapper_impl_base::get() const noexcept {
  result_wrapper_impl<T> *rwi = dynamic_cast<result_wrapper_impl<T>*>(this);
  if (rwi == nullptr) {
    return nullptr;
  }
  return &(rwi->get_impl());
}

template <typename T>
T const& result_wrapper_impl_base::get_unsafe() const noexcept {
  result_wrapper_impl<T> *rwi = static_cast<result_wrapper_impl<T>*>(this);
  return rwi->get();
}

} // namespace sona

#endif // RESULT_WRAPPER_H
