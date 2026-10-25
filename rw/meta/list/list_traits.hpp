#pragma once
#include "../core/type_list.hpp"
#include "../debug/print_type.hpp"

namespace rw::meta {

template <typename T, typename List>
struct type_list_contains;

template <typename T, typename... Ts>
struct type_list_contains<T, type_list<Ts...>>
    : std::bool_constant<(std::is_same_v<T, Ts> || ...)> {};

template <typename T, typename List>
inline constexpr bool type_list_contains_v = type_list_contains<T, List>::value;

template <typename List>
struct type_list_unique;

template <>
struct type_list_unique<type_list<>> : std::true_type {};

template <typename Head, typename... Tail>
struct type_list_unique<type_list<Head, Tail...>>
    : std::bool_constant<
          !type_list_contains_v<Head, type_list<Tail...>>
          && type_list_unique<type_list<Tail...>>::value> {};

template <typename List>
inline constexpr bool type_list_unique_v = type_list_unique<List>::value;

template <typename T, typename List>
struct index_of;

template <typename T, typename... Ts>
struct index_of<T, meta::type_list<T, Ts...>> : std::integral_constant<size_t, 0> {};

template <typename T, typename U, typename... Ts>
struct index_of<T, meta::type_list<U, Ts...>>
    : std::integral_constant<size_t, 1 + index_of<T, meta::type_list<Ts...>>::value> {};

template <typename T>
struct index_of<T, meta::type_list<>> {
    static_assert(always_false_v<T>, "Type not found in tuple");
};

template <typename T, typename List>
inline constexpr size_t index_of_v = index_of<T, List>::value;

} // namespace rw::meta