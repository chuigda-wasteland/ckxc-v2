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

#ifndef SONA_NO_DEBUG
namespace impl_74hc183 {
void panic_impl(char const* file, int line, char const* func, char const* desc);
void assert_impl(bool cond, char const* file, int line,
                 char const* func, char const* cond_str, char const* desc);
} // impl_74hc183
#endif

} // namespace sona

#ifndef SONA_NO_DEBUG

#define sona_panic(desc) { sona::impl_74hc183::panic_impl(\
                           __FILE__, __LINE__, __func__, desc); }

#define sona_assert(cond, desc) { sona::impl_74hc183::assert_impl(\
                                  cond, __FILE__, __LINE__, __func__,\
                                  #cond, desc); }

#else

#define sona_panic(desc) {;}
#define sona_assert(cond, desc) {;}

#endif

#endif // UTIL_HPP
