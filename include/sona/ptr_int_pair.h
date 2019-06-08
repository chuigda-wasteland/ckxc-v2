#ifndef PTR_INT_PAIR_H
#define PTR_INT_PAIR_H

#include "sona/util.h"

#include <cassert>
#include <cstdint>
#include <cstddef>

namespace sona {

constexpr unsigned bitmask(unsigned nbit) {
  if (nbit == 0) return 0;
  else return (1 << (nbit - 1)) + bitmask(nbit - 1);
}

constexpr std::size_t MaxBitsCompressed = 3; 

template <typename T, size_t BitsCompressed> class ptr_int_pair_owner;

template <typename T, size_t BitsCompressed>
class ptr_int_pair {
  static_assert(BitsCompressed <= MaxBitsCompressed,
                "too many bits compressed!");
  friend class ptr_int_pair_owner<T, BitsCompressed>;
public:
  enum { bit_mask = bitmask(BitsCompressed) };

  ptr_int_pair(T* ptr, unsigned value = 0) {
    sona_assert1(value < (1 << BitsCompressed), "value > BitsCompressed");
    std::uintptr_t uptr = static_cast<std::uintptr_t>(ptr);
    sona_assert1((uptr & bit_mask) == 0,
                 "this pointer is not aligned or broken!");
    uptr |= value;
    this->ptr = reinterpret_cast<T*>(uptr);
  }

  ptr_int_pair(ptr_int_pair const&) = default;
  ptr_int_pair& operator=(ptr_int_pair const&) = default;

  void set_value(unsigned value) noexcept {
    std::uintptr_t uptr = reinterpret_cast<std::uintptr_t>(ptr);
    uptr &= ~bit_mask;
    uptr |= value;
    ptr = reinterpret_cast<T*>(uptr);
  }

  unsigned get_value() const noexcept {
    std::uintptr_t uptr = reinterpret_cast<std::uintptr_t>(ptr);
    return uptr & bit_mask;
  }

  T& operator*() const noexcept {
    return *(operator->());
  }

  T* operator->() const noexcept {
    std::uintptr_t uptr = reinterpret_cast<std::uintptr_t>(ptr);
    uptr &= ~bit_mask;
    return static_cast<T*>(uptr);
  }

private:
  struct owner_construct {};
  ptr_int_pair(owner_construct, T* rawptr) {
    ptr = rawptr;
  }
  
  T* ptr;
};

template <typename T, size_t BitsCompressed>
class ptr_int_pair_owner {
  static_assert(BitsCompressed <= MaxBitsCompressed,
                "too many bits compressed!");
  
public:
  enum { bit_mask = bitmask(BitsCompressed) };

  ptr_int_pair_owner(T* ptr, unsigned value = 0) {
    sona_assert1(value < (1 << BitsCompressed), "value > BitsCompressed");
    std::uintptr_t uptr = reinterpret_cast<std::uintptr_t>(ptr);
    sona_assert1((uptr & bit_mask) == 0,
                 "this pointer is not aligned or broken!");
    uptr |= value;
    this->ptr = reinterpret_cast<T*>(uptr);
  }
  
  ptr_int_pair_owner(ptr_int_pair_owner const&) = delete;
  ptr_int_pair_owner& operator=(ptr_int_pair_owner const&) = delete;
  
  ptr_int_pair_owner(ptr_int_pair_owner&& that) {
    delete operator->();
    ptr = that->ptr;
    that->ptr = NULL;
  }
  
  ptr_int_pair_owner& operator=(ptr_int_pair_owner&& that) {
    delete operator->();
    ptr = that->ptr;
    that->ptr = NULL;
    return *this;
  }
  
  ~ptr_int_pair_owner() {
    delete operator->();
  }
  
  ptr_int_pair<T, BitsCompressed> borrow() const noexcept {
    using refty = ptr_int_pair<T, BitsCompressed>;
    using refty_oc = typename refty::owner_construct;
    return refty(refty_oc(), ptr);
  }
  
  void set_value(unsigned value) noexcept {
    std::uintptr_t uptr = reinterpret_cast<std::uintptr_t>(ptr);
    uptr &= ~bit_mask;
    uptr |= value;
    ptr = static_cast<T*>(uptr);
  }

  unsigned get_value() const noexcept {
    std::uintptr_t uptr = reinterpret_cast<std::uintptr_t>(ptr);
    return uptr & bit_mask;
  }

  T& operator*() const noexcept {
    return *(operator->());
  }

  T* operator->() const noexcept {
    std::uintptr_t uptr = reinterpret_cast<std::uintptr_t>(ptr);
    uptr &= ~bit_mask;
    return reinterpret_cast<T*>(uptr);
  }

private:
  T *ptr;
};

} // namespace sona

#endif // PTR_INT_PAIR_H
