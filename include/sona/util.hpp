#ifndef UTIL_HPP
#define UTIL_HPP

#include <memory>

namespace sona {

template <typename T, typename... Args>
void construct(T* t, Args&& ...args) {
    ::new (t) T(std::forward<Args>(args)...);
}

template <typename T>
void destroy_at(T* t) {
    t->~T();
}

} // namespace sona

#endif // UTIL_HPP
