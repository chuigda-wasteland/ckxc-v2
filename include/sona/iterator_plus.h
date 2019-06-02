#ifndef ITERATOR_PLUS_H
#define ITERATOR_PLUS_H

#include "util.h"

#include <iterator>
#include <type_traits>

namespace sona {

template <typename Number> class numeric_iterator {
  using self_type = numeric_iterator<Number>;

public:
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::make_signed_t<Number>;
  using value_type = Number;
  using reference = value_type const &;
  using pointer = value_type *;

  numeric_iterator(Number begin, Number end, Number step, Number curr)
      : begin(begin), end(end), step(step), curr(curr) {}

  reference operator*() const noexcept { return curr; }

  self_type &operator++() noexcept {
    curr += step;
    curr = curr > end ? end : curr;
    return *this;
  }

  self_type &operator--() noexcept {
    curr -= step;
    curr = curr < begin ? begin : curr;
    return *this;
  }

  self_type &operator+=(difference_type diff) noexcept {
    curr += diff;
    return *this;
  }

  self_type &operator-=(difference_type diff) noexcept {
    curr -= diff;
    return *this;
  }

  self_type operator++(int)noexcept {
    self_type t = *this;
    ++(*this);
    return t;
  }

  self_type operator--(int)noexcept {
    self_type t = *this;
    --(*this);
    return t;
  }

  self_type operator+(difference_type diff) const noexcept {
    self_type t = *this;
    t += diff;
    return t;
  }

  self_type operator-(difference_type diff) const noexcept {
    self_type t = *this;
    t -= diff;
    return t;
  }

  bool operator==(self_type const &that) const noexcept {
    sona_assert(begin == that.begin);
    sona_assert(end == that.end);
    sona_assert(step == that.step);
    return curr == that.curr;
  }

  bool operator!=(self_type const &that) const noexcept {
    return !(*this == that);
  }

  bool operator<(self_type const &that) const noexcept {
    sona_assert(begin == that.begin);
    sona_assert(end == that.end);
    sona_assert(step == that.step);
    return curr < that.curr;
  }

  bool operator>(self_type const &that) const noexcept { return that < *this; }

  bool operator<=(self_type const &that) const noexcept {
    return !(*this > that);
  }

  bool operator>=(self_type const &that) const noexcept {
    return !(*this < that);
  }

  difference_type operator-=(self_type const &that) const noexcept {
    sona_assert(begin == that.begin);
    sona_assert(end == that.end);
    sona_assert(step == that.step);
    return curr - that.curr;
  }

private:
  Number begin, end, step, curr;
};

} // namespace sona

#endif // ITERATOR_PLUS_H
