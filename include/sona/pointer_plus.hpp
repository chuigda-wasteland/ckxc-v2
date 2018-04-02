#ifndef POINTER_PLUS_HPP
#define POINTER_PLUS_HPP

#include <memory>

namespace sona {

template <typename T>
class ref_ptr {
public:
    ref_ptr(T& ref) : ptr(std::addressof(ref)) {}
    ref_ptr(T&&) = delete;

    ref_ptr(ref_ptr const&) = default;
    ref_ptr& operator= (ref_ptr const&) = default;

    operator T& () const noexcept { return *ptr; }
    T& get() const noexcept { return *ptr; }

private:
    T* ptr;
};

} // namespace sona

#endif // POINTER_PLUS_HPP
