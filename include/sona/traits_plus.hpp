#ifndef TRAITS_PLUS_HPP
#define TRAITS_PLUS_HPP

#include <memory>
#include <type_traits>

namespace sona {

namespace detail {

/// This part of code is copy-pasted from
/// @link http://en.cppreference.com/w/cpp/types/result_of

template <class T> struct is_reference_wrapper : std::false_type {};

template <class U>
struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type {};

template <class Base, class T, class Derived, class... Args>
auto INVOKE(T Base::*pmf, Derived &&ref, Args &&... args) -> std::enable_if_t<
    std::is_function<T>::value &&
        std::is_base_of<Base, typename std::decay<Derived>::type>::value,
    decltype((std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...))>;

template <class Base, class T, class RefWrap, class... Args>
auto INVOKE(T Base::*pmf, RefWrap &&ref, Args &&... args) -> std::enable_if_t<
    std::is_function<T>::value &&
        is_reference_wrapper<typename std::decay<RefWrap>::type>::value,
    decltype((ref.get().*pmf)(std::forward<Args>(args)...))>;

template <class Base, class T, class Pointer, class... Args>
auto INVOKE(T Base::*pmf, Pointer &&ptr, Args &&... args) -> std::enable_if_t<
    std::is_function<T>::value &&
        !is_reference_wrapper<typename std::decay<Pointer>::type>::value &&
        !std::is_base_of<Base, typename std::decay<Pointer>::type>::value,
    decltype(((*std::forward<Pointer>(ptr)).*
              pmf)(std::forward<Args>(args)...))>;

template <class Base, class T, class Derived>
auto INVOKE(T Base::*pmd, Derived &&ref) -> std::enable_if_t<
    !std::is_function<T>::value &&
        std::is_base_of<Base, typename std::decay<Derived>::type>::value,
    decltype(std::forward<Derived>(ref).*pmd)>;

template <class Base, class T, class RefWrap>
auto INVOKE(T Base::*pmd, RefWrap &&ref) -> std::enable_if_t<
    !std::is_function<T>::value &&
        is_reference_wrapper<typename std::decay<RefWrap>::type>::value,
    decltype(ref.get().*pmd)>;

template <class Base, class T, class Pointer>
auto INVOKE(T Base::*pmd, Pointer &&ptr) -> std::enable_if_t<
    !std::is_function<T>::value &&
        !is_reference_wrapper<typename std::decay<Pointer>::type>::value &&
        !std::is_base_of<Base, typename std::decay<Pointer>::type>::value,
    decltype((*std::forward<Pointer>(ptr)).*pmd)>;

template <class F, class... Args>
auto INVOKE(F &&f, Args &&... args) -> std::enable_if_t<
    !std::is_member_pointer<typename std::decay<F>::type>::value,
    decltype(std::forward<F>(f)(std::forward<Args>(args)...))>;

template <typename AlwaysVoid, typename, typename...> struct invoke_result {};

template <typename F, typename... Args>
struct invoke_result<decltype(void(detail::INVOKE(std::declval<F>(),
                                                  std::declval<Args>()...))),
                     F, Args...> {
  using type =
      decltype(detail::INVOKE(std::declval<F>(), std::declval<Args>()...));
};

} // namespace detail

template <class F, class... ArgTypes>
struct invoke_result : detail::invoke_result<void, F, ArgTypes...> {};

template <typename Func, typename... Args>
using invoke_result_t = typename invoke_result<Func, Args...>::type;

} // namespace sona

#endif // TRAITS_PLUS_HPP
