#pragma once
#include <type_traits>

namespace rw::meta {

template <typename... Ts>
struct type_list {};

template <typename List>
struct type_list_size;

template <typename... Ts>
struct type_list_size<type_list<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)> {};

template <typename List>
inline constexpr std::size_t type_list_size_v = type_list_size<List>::value;

template <typename List>
inline constexpr bool type_list_empty_v = type_list_size_v<List> == 0;

} // namespace rw::meta