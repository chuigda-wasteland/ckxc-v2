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

    operator ref_ptr<T const> () const noexcept {
        return ref_ptr<T const>(*ptr);
    }

    operator T& () const noexcept { return *ptr; }
    T& get() const noexcept { return *ptr; }

private:
    T* ptr;
};

template <typename T>
class owner {
public:
    owner(T* ptr) : ptr(ptr) {}
    ~owner() { delete ptr; }

    owner(owner const&) = delete;
    owner(owner&& that) : ptr(that.ptr) { that.ptr = nullptr; }

    ref_ptr<T> borrow() noexcept { return ref_ptr<T>(*ptr); }
    ref_ptr<T const> borrow() const noexcept { return ref_ptr<T>(*ptr); }

private:
    T *ptr;
};

} // namespace sona

#endif // POINTER_PLUS_HPP
