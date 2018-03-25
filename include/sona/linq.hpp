#ifndef LINQ_HPP
#define LINQ_HPP

#include "optional.hpp"
#include <type_traits>

namespace sona {

namespace linq_impl {

template <typename Base, typename T>
using max_iter_tag = std::conditional_t<std::is_base_of<Base,T>::value, T,Base>;

template <typename Base, typename T>
using min_iter_tag = std::conditional_t<std::is_base_of<Base,T>::value, Base,T>;

template <typename Iter, typename Transform>
class transform_iterator {
    using self_type = transform_iterator<Iter, Transform>;

public:
    using iterator_category =
          typename std::iterator_traits<Iter>::iterator_category;
    using difference_type =
          typename std::iterator_traits<Iter>::difference_type;
    using value_type = typename std::iterator_traits<Iter>::value_type;
    using reference = typename std::iterator_traits<Iter>::reference;
    using pointer = typename std::iterator_traits<Iter>::pointer;

    transform_iterator(Iter iter, Transform transform) noexcept :
        iter(iter), transform(std::move(transform)) {}

    decltype(auto) operator* () { return transform(*iter); }

    self_type& operator++ () noexcept { ++iter; return *this; }

    self_type& operator-- () noexcept { --iter; return *this; }

    self_type operator++ (int) noexcept {
        self_type ret = *this;
        ++this;
        return ret;
    }

    self_type operator-- (int) noexcept {
        self_type ret = *this;
        --this;
        return ret;
    }

    self_type& operator+= (difference_type diff) noexcept
        { iter += diff; return *this; }

    self_type& operator-= (difference_type diff) noexcept
        { iter -= diff; return *this; }

    self_type operator+ (difference_type diff) const noexcept {
        self_type t = iter; t += diff; return t;
    }

    self_type operator- (difference_type diff) const noexcept {
        self_type t = iter; t -= diff; return t;
    }

    bool operator== (self_type const& that) const noexcept {
        return iter == that.iter;
    }

    bool operator!= (self_type const& that) const noexcept {
        return *this != that;
    }

    bool operator< (self_type const& that) const noexcept {
        return iter < that.iter;
    }

    bool operator> (self_type const& that) const noexcept {
        return that < *this;
    }

    bool operator<= (self_type const& that) const noexcept {
        return !(*this > that);
    }

    bool operator>= (self_type const& that) const noexcept {
        return !(*this < that);
    }

    difference_type operator- (self_type const& that) const noexcept {
        return iter - that.iter;
    }

private:
    Iter iter;
    Transform transform;
};

template <typename Iter, typename Filter>
class filter_iterator {
    using self_type = filter_iterator<Iter, Filter>;

public:
    using iterator_category = typename std::forward_iterator_tag;
    using difference_type =
          typename std::iterator_traits<Iter>::difference_type;
    using value_type = typename std::iterator_traits<Iter>::value_type;
    using reference = typename std::iterator_traits<Iter>::reference;
    using pointer = typename std::iterator_traits<Iter>::pointer;

    filter_iterator(Iter iter, Iter end, Filter filter) noexcept :
        iter(iter), end(end), filter(std::move(filter)) {
        while (iter != end && !filter(*iter)) ++iter;
    }

    reference operator* () noexcept { return *iter; }

    bool operator== (self_type const& other) const noexcept {
        return iter == other.iter;
    }

    bool operator!= (self_type const& other) const noexcept {
        return !(*this == other);
    }

    self_type& operator++ () noexcept {
        while (iter != end && !filter(*iter)) ++iter; return *this;
    }

    self_type operator++ (int) noexcept {
        self_type t = *this; ++(*this); return t;
    }

private:
    Iter iter, end;
    Filter filter;
};

template <typename Iter, typename Filter>
class bidirectional_filter_iterator {
    using self_type = bidirectional_filter_iterator<Iter, Filter>;

public:
    using iterator_category = typename std::bidirectional_iterator_tag;
    using difference_type = typename std::iterator_traits<Iter>::difference_type;
    using value_type = typename std::iterator_traits<Iter>::value_type;
    using reference = typename std::iterator_traits<Iter>::reference;
    using pointer = typename std::iterator_traits<Iter>::pointer;

    bidirectional_filter_iterator(Iter iter, Iter begin,
                                  Iter end, Filter filter) noexcept :
        iter(iter), begin(begin), end(end), filter(std::move(filter)) {
        while (begin != end && !filter(*begin)) ++begin;
        while (iter != end && !filter(*iter)) ++iter;
    }

    reference operator* () noexcept { return *iter; }

    bool operator== (self_type const& other) const noexcept {
        return iter == other.iter;
    }

    bool operator!= (self_type const& other) const noexcept {
        return !(*this == other);
    }

    self_type& operator++ () noexcept {
        while (iter != end && !filter(*iter)) ++iter; return *this;
    }

    self_type& operator-- () noexcept {
        while (iter != begin && !filter(*iter)) --iter; return *this;
    }

    self_type operator++ (int) noexcept {
        self_type t = *this; ++(*this); return t;
    }

    self_type operator-- (int) noexcept {
        self_type t = *this; ++(*this); return t;
    }

private:
    Iter iter, begin, end;
    Filter filter;
};

template <typename Iter1, typename Iter2>
class concat_iterator {
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
    using iterator_category = min_iter_tag<iterator_category1,
                                           iterator_category2>;
    using difference_type = std::common_type_t<difference_type1,
                                               difference_type2>;
    using value_type = std::common_type_t<value_type1, value_type2>;
    using reference = std::common_type_t<reference1, reference2>;
    using pointer = std::common_type_t<pointer1, pointer2>;

    concat_iterator(Iter1 begin1, Iter1 end1, Iter1 curr1,
                    Iter2 begin2, Iter2 end2, Iter2 curr2) noexcept :
        begin1(begin1), end1(end1), curr1(curr1),
        begin2(begin2), end2(end2), curr2(curr2) {}

    reference operator* () noexcept {
        return curr1 == end1 ? (*curr2) : (*curr1);
    }

    self_type& operator++ () noexcept {
        ++ (curr1 == end1 ? curr2 : curr1); return *this;
    }

    self_type& operator-- () noexcept {
        -- (curr2 == begin2 ? curr1 : curr2); return *this;
    }

    self_type& operator+= (difference_type diff) noexcept {
        auto const diff1 = curr1 - end1;
        if (diff1 < diff) { curr1 += diff1; curr2 += (diff - diff1); }
        else { curr1 += diff; }
        return *this;
    }

    self_type& operator-= (difference_type diff) noexcept {
        auto const diff1 = curr2 - begin2;
        if (diff1 < diff) { curr2 -= diff1; curr1 -= (diff - diff1); }
        else { curr2 -= diff; }
        return *this;
    }

    self_type operator--(int) noexcept { self_type t; --(*this); return t; }

    self_type operator++(int) noexcept { self_type t; ++(*this); return t; }

    self_type operator+ (difference_type diff) const noexcept {
        self_type t; t += diff; return t;
    }

    self_type operator- (difference_type diff) const noexcept {
        self_type t; t -= diff; return t;
    }

    bool operator== (self_type const& that) noexcept {
        return (curr1 == that.curr1) && (curr2 == that.curr2);
    }

    bool operator!= (self_type const& that) noexcept {
        return !(*this == that);
    }

    bool operator< (self_type const& that) noexcept {
        if (curr1 == end1) {
            if (that.curr1 == that.end1) return curr2 < that.curr2;
            else return false;
        }
        else {
            return curr1 < that.curr1;
        }
    }

    bool operator> (self_type const& that) noexcept {
        return that < *this;
    }

    bool operator<= (self_type const& that) noexcept {
        return !(*this > that);
    }

    bool operator>= (self_type const& that) noexcept {
        return !(*this < that);
    }

    difference_type operator- (self_type const& that) noexcept {
        return curr1 == end1 ? curr2 - that.curr2 : curr1 - that.curr1;
    }

private:
    Iter1 begin1, end1, curr1;
    Iter2 begin2, end2, curr2;
};

template <typename Iter1, typename Iter2>
class zip_iterator {
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
    using iterator_category = min_iter_tag<iterator_category1,
                                           iterator_category2>;
    using difference_type = std::common_type_t<difference_type1,
                                               difference_type2>;
    using value_type = std::pair<reference1, reference2>;
    using reference = value_type;
    using pointer = value_type*;

    zip_iterator(Iter1 iter1, Iter2 iter2) noexcept :
        iter1(iter1), iter2(iter2) {}

    reference operator* () noexcept {
        return std::make_pair(*iter1, *iter2);
    }

    self_type& operator++ () noexcept {
        ++iter1; ++iter2; return *this;
    }

    self_type& operator-- () noexcept {
        --iter1; --iter2; return *this;
    }

    self_type& operator+= (difference_type diff) noexcept {
        iter1 += diff; iter2 += diff; return *this;
    }

    self_type& operator-= (difference_type diff) noexcept {
        iter1 -= diff; iter2 -= diff; return *this;
    }

    self_type operator++ (int) noexcept {
        self_type t = *this; ++(*this); return t;
    }

    self_type operator-- (int) noexcept {
        self_type t = *this; --(*this); return t;
    }

    self_type operator+ (difference_type diff) const noexcept {
        self_type t = *this; t += diff; return t;
    }

    self_type operator- (difference_type diff) const noexcept {
        self_type t = *this; t -= diff; return t;
    }

    bool operator== (self_type const& that) const noexcept {
        return iter1 == that.iter1;
    }

    bool operator!= (self_type const& that) const noexcept {
        return !(*this == that);
    }

    bool operator< (self_type const& that) const noexcept {
        return iter1 < that.iter1;
    }

    bool operator> (self_type const& that) const noexcept {
        return that < *this;
    }

    bool operator<= (self_type const& that) const noexcept {
        return !(*this > that);
    }

    bool operator>= (self_type const& that) const noexcept {
        return !(*this < that);
    }

    difference_type operator- (self_type const& that) const noexcept {
        return iter1 - that.iter1;
    }

private:
    Iter1 iter1;
    Iter2 iter2;
};

} // namespace linq_impl

} // namespace sona

#endif // LINQ_HPP
