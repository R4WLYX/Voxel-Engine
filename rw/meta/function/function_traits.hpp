#pragma once
#include "../core/type_list.hpp"

namespace rw::meta {

template <typename Func, typename = void>
struct function_traits;

// Normal function
template <typename Ret, typename... Args>
struct function_traits<Ret(Args...), void> {
    using return_type = Ret;
    using args = type_list<Args...>;
    static constexpr size_t argument_count = sizeof...(Args);
};

// Function pointer
template <typename Ret, typename... Args>
struct function_traits<Ret (*)(Args...), void> {
    using return_type = Ret;
    using args = type_list<Args...>;
    static constexpr size_t argument_count = sizeof...(Args);
};

// Function reference
template <typename Ret, typename... Args>
struct function_traits<Ret (&)(Args...), void> {
    using return_type = Ret;
    using args = type_list<Args...>;
    static constexpr size_t argument_count = sizeof...(Args);
};

// Function rvalue reference
template <typename Ret, typename... Args>
struct function_traits<Ret (&&)(Args...), void> {
    using return_type = Ret;
    using args = type_list<Args...>;
    static constexpr size_t argument_count = sizeof...(Args);
};

// Lambdas
template <typename Func>
struct function_traits<Func, std::void_t<decltype(&Func::operator())>>
    : function_traits<decltype(&Func::operator())> {};

// Member function
template <typename C, typename Ret, typename... Args>
struct function_traits<Ret (C::*)(Args...) const, void> {
    using return_type = Ret;
    using args = type_list<Args...>;
    static constexpr size_t argument_count = sizeof...(Args);
};

// Non-const member function
template <typename C, typename Ret, typename... Args>
struct function_traits<Ret (C::*)(Args...), void> {
    using return_type = Ret;
    using args = type_list<Args...>;
    static constexpr size_t argument_count = sizeof...(Args);
};

template <typename Func>
using function_args_t =
    typename function_traits<std::remove_cvref_t<Func>>::args;

template <typename Func>
inline constexpr size_t function_args_count_v =
    function_traits<std::remove_cvref_t<Func>>::argument_count;

template <typename Func>
using function_return_t = typename function_traits<std::remove_cvref_t<Func>>::return_type;

template <auto Func>
using function_args_of_t = function_args_t<decltype(Func)>;

template <auto Func>
inline constexpr size_t function_args_count_of_v = function_args_count_v<decltype(Func)>;

template <auto Func>
using function_return_of_t = function_return_t<decltype(Func)>;

} // namespace rw::meta