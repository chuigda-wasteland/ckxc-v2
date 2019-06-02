#ifndef RANGE_HPP
#define RANGE_HPP

#include <iterator>

namespace sona {

template <typename Iterator> class iterator_range {
public:
  using iterator = Iterator;
  using reverse_iterator = std::reverse_iterator<Iterator>;

  using difference_type =
      typename std::iterator_traits<Iterator>::difference_type;
  using value_type = typename std::iterator_traits<Iterator>::value_type;
  using reference = typename std::iterator_traits<Iterator>::reference;
  using pointer = typename std::iterator_traits<Iterator>::pointer;

  iterator_range(Iterator first, Iterator last) noexcept
      : first(first), last(last) {}
  iterator_range(iterator_range const &that) noexcept
      : iterator_range(that.first, that.last) {}

  iterator begin() const noexcept { return first; }
  iterator end() const noexcept { return last; }

  reverse_iterator rbegin() const noexcept { return reverse_iterator(last); }
  reverse_iterator rend() const noexcept { return reverse_iterator(first); }

  std::size_t size() const noexcept { return std::distance(first, last); }

private:
  Iterator const first;
  Iterator const last;
};

} // namespace sona

#endif // RANGE_HPP
