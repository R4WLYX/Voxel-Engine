#pragma once
#include <tuple>

#include "../core/type_list.hpp"

namespace rw::meta {

template <typename List>
struct to_tuple;

template <typename... Ts>
struct to_tuple<type_list<Ts...>> {
    using type = std::tuple<Ts...>;
};

template <typename List>
using to_tuple_t = typename to_tuple<List>::type;

template <typename Tuple>
struct to_type_list;

template <typename... Ts>
struct to_type_list<std::tuple<Ts...>> {
    using type = type_list<Ts...>;
};

template <typename Tuple>
using to_type_list_t = typename to_type_list<Tuple>::type;

} // namespace rw::meta