#ifndef RANGE_HPP
#define RANGE_HPP

#include <iterator>

namespace sona {

template <typename Iterator>
class iterator_range {
public:
    using size_type        =  std::size_t;
    using difference_type  =  std::ptrdiff_t;

    using iterator         =  Iterator;
    using reverse_iterator =  std::reverse_iterator<Iterator>;
    using value_type       =  typename
                              std::iterator_traits<Iterator>::value_type;
    using reference        =  value_type&;
    using const_refernece  =  value_type const&;

    using pointe           =  value_type*;
    using const_pointer    =  value_type const*;

    iterator_range(Iterator first, Iterator last) :
        first(first), last(last) {}
    iterator_range(iterator_range const& that) :
        iterator_range(that.first, that.last) {}

    iterator begin() const { return first; }
    iterator end() const { return last; }

    iterator rbegin() const { return reverse_iterator(last); }
    iterator rend() const { return reverse_iterator(first); }

private:
    Iterator const first;
    Iterator const last;
};

} // namespace sona

#endif // RANGE_HPP
