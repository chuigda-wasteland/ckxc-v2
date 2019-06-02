#ifndef LINQ_HPP
#define LINQ_HPP

#include "iterator_plus.hpp"
#include "optional.hpp"
#include "range.hpp"
#include "traits_plus.hpp"
#include "util.hpp"

/// @attention Sona is for ckx, always for ckx and only for ckx.
namespace sona {

namespace linq_impl {

template <typename Base, typename T>
using max_iter_tag =
    std::conditional_t<std::is_base_of<Base, T>::value, T, Base>;

template <typename Base, typename T>
using min_iter_tag =
    std::conditional_t<std::is_base_of<Base, T>::value, Base, T>;

template <typename Iter, typename Transform> class transform_iterator {
  using self_type = transform_iterator<Iter, Transform>;

public:
  using iterator_category =
    typename std::iterator_traits<Iter>::iterator_category;
  using difference_type = typename std::iterator_traits<Iter>::difference_type;
  using value_type =
    decltype(std::declval<Transform>()(*std::declval<Iter>()));
  using reference = value_type;
  using pointer = std::add_pointer_t<value_type>;

  transform_iterator(Iter iter, Transform transform) noexcept
      : iter(iter), transform(std::move(transform)) {}

  decltype(auto) operator*() { return transform(*iter); }

  self_type &operator++() noexcept {
    ++iter;
    return *this;
  }

  self_type &operator--() noexcept {
    --iter;
    return *this;
  }

  self_type operator++(int)noexcept {
    self_type ret = *this;
    ++this;
    return ret;
  }

  self_type operator--(int)noexcept {
    self_type ret = *this;
    --this;
    return ret;
  }

  self_type &operator+=(difference_type diff) noexcept {
    iter += diff;
    return *this;
  }

  self_type &operator-=(difference_type diff) noexcept {
    iter -= diff;
    return *this;
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
    return iter == that.iter;
  }

  bool operator!=(self_type const &that) const noexcept {
    return !(*this == that);
  }

  bool operator<(self_type const &that) const noexcept {
    return iter < that.iter;
  }

  bool operator>(self_type const &that) const noexcept { return that < *this; }

  bool operator<=(self_type const &that) const noexcept {
    return !(*this > that);
  }

  bool operator>=(self_type const &that) const noexcept {
    return !(*this < that);
  }

  difference_type operator-(self_type const &that) const noexcept {
    return iter - that.iter;
  }

private:
  Iter iter;
  Transform transform;
};

template <typename Iter, typename Filter> class filter_iterator {
  using self_type = filter_iterator<Iter, Filter>;

public:
  using iterator_category = typename std::forward_iterator_tag;
  using difference_type = typename std::iterator_traits<Iter>::difference_type;
  using value_type = typename std::iterator_traits<Iter>::value_type;
  using reference = typename std::iterator_traits<Iter>::reference;
  using pointer = typename std::iterator_traits<Iter>::pointer;

  filter_iterator(Iter iter_, Iter end_, Filter filter_) noexcept
      : iter(iter_), end(end_), filter(std::move(filter_)) {
    while (iter != end && !filter(*iter)) ++iter;
  }

  reference operator*() noexcept {
    sona_assert(iter != end);
    return *iter;
  }

  bool operator==(self_type const &other) const noexcept {
    sona_assert(end == other.end);
    return iter == other.iter;
  }

  bool operator!=(self_type const &other) const noexcept {
    return !(*this == other);
  }

  self_type &operator++() noexcept {
    if (iter != end) ++iter;
    while (iter != end && !filter(*iter)) ++iter;
    return *this;
  }

  self_type operator++(int)noexcept {
    self_type t = *this;
    ++(*this);
    return t;
  }

private:
  Iter iter, end;
  Filter filter;
};

template <typename Iter, typename Filter> class bidirectional_filter_iterator {
  using self_type = bidirectional_filter_iterator<Iter, Filter>;

public:
  using iterator_category = typename std::bidirectional_iterator_tag;
  using difference_type = typename std::iterator_traits<Iter>::difference_type;
  using value_type = typename std::iterator_traits<Iter>::value_type;
  using reference = typename std::iterator_traits<Iter>::reference;
  using pointer = typename std::iterator_traits<Iter>::pointer;

  bidirectional_filter_iterator(Iter iter_, Iter begin_, Iter end_,
                                Filter filter) noexcept
      : iter(iter_), begin(begin_), end(end_), filter(std::move(filter)) {
    while (begin != end && !filter(*begin)) ++begin;
    while (iter != end && !filter(*iter)) ++iter;
  }

  reference operator*() noexcept { return *iter; }

  bool operator==(self_type const &other) const noexcept {
    sona_assert(begin == other.begin);
    sona_assert(end == other.end);
    return iter == other.iter;
  }

  bool operator!=(self_type const &other) const noexcept {
    return !(*this == other);
  }

  self_type &operator++() noexcept {
    if (iter != end) ++iter;
    while (iter != end && !filter(*iter)) ++iter;
    return *this;
  }

  self_type &operator--() noexcept {
    if (iter != begin) --iter;
    while (iter != begin && !filter(*iter)) --iter;
    return *this;
  }

  self_type operator++(int)noexcept {
    self_type t = *this;
    ++(*this);
    return t;
  }

  self_type operator--(int)noexcept {
    self_type t = *this;
    ++(*this);
    return t;
  }

private:
  Iter iter, begin, end;
  Filter filter;
};

template <typename Iter1, typename Iter2> class concat_iterator {
  using self_type = concat_iterator<Iter1, Iter2>;
  using iterator_category1 =
      typename std::iterator_traits<Iter1>::iterator_category;
  using iterator_category2 =
      typename std::iterator_traits<Iter2>::iterator_category;
  using difference_type1 =
      typename std::iterator_traits<Iter1>::difference_type;
  using difference_type2 =
      typename std::iterator_traits<Iter2>::difference_type;
  using value_type1 = typename std::iterator_traits<Iter1>::value_type;
  using value_type2 = typename std::iterator_traits<Iter2>::value_type;
  using reference1 = typename std::iterator_traits<Iter1>::reference;
  using reference2 = typename std::iterator_traits<Iter2>::reference;
  using pointer1 = typename std::iterator_traits<Iter1>::pointer;
  using pointer2 = typename std::iterator_traits<Iter2>::pointer;

public:
  using iterator_category =
      min_iter_tag<iterator_category1, iterator_category2>;
  using difference_type =
      std::common_type_t<difference_type1, difference_type2>;
  using value_type = std::common_type_t<value_type1, value_type2>;
  using reference = std::common_type_t<reference1, reference2>;
  using pointer = std::common_type_t<pointer1, pointer2>;

  concat_iterator(Iter1 begin1, Iter1 end1, Iter1 curr1, Iter2 begin2,
                  Iter2 end2, Iter2 curr2) noexcept
      : begin1(begin1), end1(end1), curr1(curr1), begin2(begin2), end2(end2),
        curr2(curr2) {}

  reference operator*() noexcept {
    return (curr1 == end1) ? (*curr2) : (*curr1);
  }

  self_type &operator++() noexcept {
    if (curr1 == end1)
      ++curr2;
    else
      ++curr1;
    return *this;
  }

  self_type &operator--() noexcept {
    if (curr2 == begin2)
      --curr1;
    else
      --curr2;
    return *this;
  }

  self_type &operator+=(difference_type diff) noexcept {
    auto const diff1 = curr1 - end1;
    if (diff1 < diff) {
      curr1 += diff1;
      curr2 += (diff - diff1);
    } else {
      curr1 += diff;
    }
    return *this;
  }

  self_type &operator-=(difference_type diff) noexcept {
    auto const diff1 = curr2 - begin2;
    if (diff1 < diff) {
      curr2 -= diff1;
      curr1 -= (diff - diff1);
    } else {
      curr2 -= diff;
    }
    return *this;
  }

  self_type operator--(int)noexcept {
    self_type t;
    --(*this);
    return t;
  }

  self_type operator++(int)noexcept {
    self_type t;
    ++(*this);
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
    sona_assert(begin1 == that.begin1);
    sona_assert(end1 == that.end1);
    sona_assert(begin2 == that.begin2);
    sona_assert(end2 == that.end2);
    return (curr1 == that.curr1) && (curr2 == that.curr2);
  }

  bool operator!=(self_type const &that) const noexcept {
    return !(*this == that);
  }

  bool operator<(self_type const &that) const noexcept {
    sona_assert(begin1 == that.begin1);
    sona_assert(end1 == that.end1);
    sona_assert(begin1 == that.begin2);
    sona_assert(end1 == that.end2);
    if (curr1 == end1) {
      if (that.curr1 == that.end1)
        return curr2 < that.curr2;
      else
        return false;
    } else {
      return curr1 < that.curr1;
    }
  }

  bool operator>(self_type const &that) const noexcept { return that < *this; }

  bool operator<=(self_type const &that) const noexcept {
    return !(*this > that);
  }

  bool operator>=(self_type const &that) const noexcept {
    return !(*this < that);
  }

  difference_type operator-(self_type const &that) const noexcept {
    sona_assert(begin1 == that.begin1);
    sona_assert(end1 == that.end1);
    sona_assert(begin1 == that.begin2);
    sona_assert(end1 == that.end2);
    return curr1 == end1 ? curr2 - that.curr2 : curr1 - that.curr1;
  }

private:
  Iter1 begin1, end1, curr1;
  Iter2 begin2, end2, curr2;
};

template <typename Iter1, typename Iter2> class zip_iterator {
  using self_type = zip_iterator<Iter1, Iter2>;
  using iterator_category1 =
      typename std::iterator_traits<Iter1>::iterator_category;
  using iterator_category2 =
      typename std::iterator_traits<Iter2>::iterator_category;
  using difference_type1 =
      typename std::iterator_traits<Iter1>::difference_type;
  using difference_type2 =
      typename std::iterator_traits<Iter2>::difference_type;
  using value_type1 = typename std::iterator_traits<Iter1>::value_type;
  using value_type2 = typename std::iterator_traits<Iter2>::value_type;
  using reference1 = typename std::iterator_traits<Iter1>::reference;
  using reference2 = typename std::iterator_traits<Iter2>::reference;
  using pointer1 = typename std::iterator_traits<Iter1>::pointer;
  using pointer2 = typename std::iterator_traits<Iter2>::pointer;

public:
  using iterator_category =
      min_iter_tag<iterator_category1, iterator_category2>;
  using difference_type =
      std::common_type_t<difference_type1, difference_type2>;
  using value_type = std::pair<value_type1, value_type2>;
  using reference = value_type;
  using pointer = value_type *;

  zip_iterator(Iter1 iter1, Iter2 iter2) noexcept
      : iter1(iter1), iter2(iter2) {}

  reference operator*() noexcept { return std::make_pair(*iter1, *iter2); }

  self_type &operator++() noexcept {
    ++iter1;
    ++iter2;
    return *this;
  }

  self_type &operator--() noexcept {
    --iter1;
    --iter2;
    return *this;
  }

  self_type &operator+=(difference_type diff) noexcept {
    iter1 += diff;
    iter2 += diff;
    return *this;
  }

  self_type &operator-=(difference_type diff) noexcept {
    iter1 -= diff;
    iter2 -= diff;
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
    sona_assert((iter1 == that.iter1 && iter2 == that.iter2) ||
                !(iter1 == that.iter1 && iter2 == that.iter2));
    return iter1 == that.iter1;
  }

  bool operator!=(self_type const &that) const noexcept {
    return !(*this == that);
  }

  bool operator<(self_type const &that) const noexcept {
    sona_assert((iter1 < that.iter1 && iter2 < that.iter2) ||
                !(iter1 < that.iter1 && iter2 < that.iter2));
    return iter1 < that.iter1;
  }

  bool operator>(self_type const &that) const noexcept { return that < *this; }

  bool operator<=(self_type const &that) const noexcept {
    return !(*this > that);
  }

  bool operator>=(self_type const &that) const noexcept {
    return !(*this < that);
  }

  difference_type operator-(self_type const &that) const noexcept {
    sona_assert((iter1 - that.iter1) == (iter2 - that.iter2));
    return iter1 - that.iter1;
  }

private:
  Iter1 iter1;
  Iter2 iter2;
};

} // namespace linq_impl

template <typename Iterator>
class linq_enumerable : public iterator_range<Iterator> {
  using self_type = linq_enumerable<Iterator>;
  using base_type = iterator_range<Iterator>;
  friend class linq;

public:
  template <typename Transform> auto transform(Transform transform) noexcept {
    using transform_iterator =
        linq_impl::transform_iterator<Iterator, Transform>;
    return linq_enumerable<transform_iterator>(
        transform_iterator(base_type::begin(), transform),
        transform_iterator(base_type::end(), transform));
  }

  template <typename Filter> auto filter(Filter filter) noexcept {
    using filter_iterator = linq_impl::filter_iterator<Iterator, Filter>;
    return linq_enumerable<filter_iterator>(
        filter_iterator(base_type::begin(), base_type::end(), filter),
        filter_iterator(base_type::end(), base_type::end(), filter));
  }

  template <typename Filter> auto filter1(Filter filter) noexcept {
    using filter_iterator =
        linq_impl::bidirectional_filter_iterator<Iterator, Filter>;
    return linq_enumerable<filter_iterator>(
        filter_iterator(base_type::begin(), base_type::begin(),
                        base_type::end(), filter),
        filter_iterator(base_type::end(), base_type::begin(), base_type::end(),
                        filter));
  }

  template <typename Iterator1>
  auto concat_with(linq_enumerable<Iterator1> const &that) noexcept {
    using concat_iterator = linq_impl::concat_iterator<Iterator, Iterator1>;
    return linq_enumerable<concat_iterator>(
        concat_iterator(base_type::begin(), base_type::end(),
                        base_type::begin(), that.begin(), that.end(),
                        that.begin()),
        concat_iterator(base_type::begin(), base_type::end(), base_type::end(),
                        that.begin(), that.end(), that.end()));
  }

  template <typename Iterator1>
  auto zip_with(linq_enumerable<Iterator1> const &that) noexcept {
    using zip_iterator = linq_impl::zip_iterator<Iterator, Iterator1>;
    return linq_enumerable<zip_iterator>(
        zip_iterator(base_type::begin(), that.begin()),
        zip_iterator(base_type::end(), that.end()));
  }

  auto reverse() noexcept {
    return linq_enumerable<typename base_type::reverse_iterator>(
        base_type::rbegin(), base_type::rend());
  }

  auto slice(typename base_type::difference_type from,
             typename base_type::difference_type to = 0) {
    sona_assert1(from >= 0, "slicing from negative index");
    if (to <= 0)
      return self_type(base_type::begin() + from, base_type::end() + to);
    else
      return self_type(base_type::begin() + from, base_type::begin() + to);
  }

  linq_enumerable(Iterator begin, Iterator end) : base_type(begin, end) {}
};

class linq {
public:
  template <typename Number>
  static auto numeric_range(Number start, Number end, Number step = 1) {
    using iterator = numeric_iterator<Number>;
    return linq_enumerable<iterator>(iterator(start, end, step, start),
                                     iterator(start, end, step, end));
  }

  template <typename Container>
  static auto from_container(Container &&container) {
    using iterator = decltype(std::forward<Container>(container).begin());
    return linq_enumerable<iterator>(std::forward<Container>(container).begin(),
                                     std::forward<Container>(container).end());
  }

  template <typename Iterator>
  static auto from_iterator(Iterator begin, Iterator end) {
    return linq_enumerable<Iterator>(begin, end);
  }
};

} // namespace sona

#endif // LINQ_HPP
