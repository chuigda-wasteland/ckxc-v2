#ifndef EITHER_HPP
#define EITHER_HPP

#include <memory>
#include <algorithm>
#include "util.hpp"

namespace sona {

template <typename T1, typename T2>
class either {
public:
    struct t1_tag{};
    struct t2_tag{};

    either(T1 const& value) {
        construct<T1>(reinterpret_cast<T1*>(&storage), value);
        status = has_t1;
    }

    either(T2 const& value) {
        construct<T2>(reinterpret_cast<T2*>(&storage), value);
        status = has_t2;
    }

    either(T1&& value) {
        construct<T1>(reinterpret_cast<T1*>(&storage), std::move(value));
        status = has_t1;
    }

    either(T2&& value) {
        construct<T2>(reinterpret_cast<T2*>(&storage), std::move(value));
        status = has_t2;
    }

    template <typename ...Args>
    either(t1_tag, Args&& ...args) {
        construct<T1>(reinterpret_cast<T1*>(&storage),
                      std::forward<Args>(args)...);
        status = has_t1;
    }

    template <typename ...Args>
    either(t2_tag, Args&& ...args) {
        construct<T2>(reinterpret_cast<T2*>(&storage),
                      std::forward<Args>(args)...);
        status = has_t2;
    }

    T1& as_t1() {
        sona_assert1(status == has_t1, "this either does not contains T1");
        return *reinterpret_cast<T1*>(&storage);
    }

    T2& as_t2() {
        sona_assert1(status == has_t2, "this either does not contains T2");
        return *reinterpret_cast<T2*>(&storage);
    }

    T1 const& as_t1() const {
        sona_assert1(status == has_t1, "this either does not contains T1");
        return *reinterpret_cast<T1 const*>(&storage);
    }

    T2 const& as_t2() const {
        sona_assert1(status == has_t2, "this either does not contains T2");
        return *reinterpret_cast<T2 const*>(&storage);
    }

    either(either const&) = delete;
    either(either&& that) : status(that.status) {
        if (status == has_t1)
            construct<T1>(
                reinterpret_cast<T1*>(&storage), std::move(that.as_t1()));
        else if (status == has_t2)
            construct<T2>(
                reinterpret_cast<T2*>(&storage), std::move(that.as_t2()));
        else
            sona_unreachable();
    }

private:
    std::aligned_storage_t<std::max(sizeof(T1), sizeof(T2)),
                           std::max(alignof(T1), alignof(T2))> storage;
    enum st_status { has_t1, has_t2 } status;
};

} // namespace sona

#endif // EITHER_HPP
