#ifndef OPTIONAL_H
#define OPTIONAL_H

#include "util.h"
#include <type_traits>

namespace sona {

struct empty_optional {
  explicit constexpr empty_optional() = default;
};

template <typename T> class optional {
public:
  optional(const T &v) : is_value(true) {
    construct(reinterpret_cast<T *>(&t), v);
  }

  optional(T &&v) : is_value(true) {
    construct(reinterpret_cast<T *>(&t), std::move(v));
  }

  template <typename... Args> optional(Args &&... args) : is_value(true) {
    construct<T>(reinterpret_cast<T *>(&t), std::forward<Args>(args)...);
  }

  optional(empty_optional) : is_value(false) {}

  bool has_value() const { return is_value; }
  T &value() { return reinterpret_cast<T &>(t); }
  T const &value() const { return reinterpret_cast<T const &>(t); }

private:
  std::aligned_storage_t<sizeof(T), alignof(T)> t;
  bool is_value;
};

} // namespace sona

#endif // OPTIONAL_H
