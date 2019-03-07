#ifndef EITHER_HPP
#define EITHER_HPP

#include "util.hpp"
#include <algorithm>
#include <memory>

namespace sona {

template <typename T1, typename T2> class either {
public:
  struct t1_tag {};
  struct t2_tag {};

  either(T1 const &value) {
    construct<T1>(reinterpret_cast<T1 *>(&storage), value);
    status = has_t1;
  }

  either(T2 const &value) {
    construct<T2>(reinterpret_cast<T2 *>(&storage), value);
    status = has_t2;
  }

  either(T1 &&value) {
    construct<T1>(reinterpret_cast<T1 *>(&storage), std::move(value));
    status = has_t1;
  }

  either(T2 &&value) {
    construct<T2>(reinterpret_cast<T2 *>(&storage), std::move(value));
    status = has_t2;
  }

  template <typename... Args> either(t1_tag, Args &&... args) {
    construct<T1>(reinterpret_cast<T1 *>(&storage),
                  std::forward<Args>(args)...);
    status = has_t1;
  }

  template <typename... Args> either(t2_tag, Args &&... args) {
    construct<T2>(reinterpret_cast<T2 *>(&storage),
                  std::forward<Args>(args)...);
    status = has_t2;
  }

  bool contains_t1() const noexcept { return status == has_t1; }
  bool contains_t2() const noexcept { return status == has_t2; }

  T1 &as_t1() {
    sona_assert1(status == has_t1, "this either does not contains T1");
    return *reinterpret_cast<T1 *>(&storage);
  }

  T2 &as_t2() {
    sona_assert1(status == has_t2, "this either does not contains T2");
    return *reinterpret_cast<T2 *>(&storage);
  }

  T1 const &as_t1() const {
    sona_assert1(status == has_t1, "this either does not contains T1");
    return *reinterpret_cast<T1 const *>(&storage);
  }

  T2 const &as_t2() const {
    sona_assert1(status == has_t2, "this either does not contains T2");
    return *reinterpret_cast<T2 const *>(&storage);
  }

  either(either const &) = delete;
  either(either &&that) : status(that.status) {
    if (status == has_t1)
      construct<T1>(reinterpret_cast<T1 *>(&storage), std::move(that.as_t1()));
    else if (status == has_t2)
      construct<T2>(reinterpret_cast<T2 *>(&storage), std::move(that.as_t2()));
    else
      sona_unreachable();
  }

  ~either() noexcept {
    if (status == has_t1) {
      destroy_at<T1>(reinterpret_cast<T1*>(&storage));
    }
    else {
      destroy_at<T2>(reinterpret_cast<T2*>(&storage));
    }
  }

  template <typename T>
  void set(T&& t) {
    ~this();
    mark_status_intern<T>();
    construct<T>(reinterpret_cast<T1>(&storage), std::forward<T>(t));
  }

private:
  template <typename T>
  std::enable_if_t<std::is_same<T, T1>::value, void>
  mark_status_intern() noexcept { status = has_t1; }

  template <typename T>
  std::enable_if_t<std::is_same<T, T2>::value, void>
  mark_status_intern() noexcept { status = has_t2; }

  std::aligned_storage_t<std::max(sizeof(T1), sizeof(T2)),
                         std::max(alignof(T1), alignof(T2))>
      storage;
  enum st_status { has_t1, has_t2 } status;
};

} // namespace sona

#endif // EITHER_HPP
