#pragma once
#include "../core/type_less.hpp"
#include "../core/type_list.hpp"
#include "list_traits.hpp"

namespace rw::meta {

namespace detail {

    template <typename T, typename List>
    struct prepend;

    template <typename T, typename... Ts>
    struct prepend<T, type_list<Ts...>> {
        using type = type_list<T, Ts...>;
    };

    template <typename T, typename List>
    using prepend_t = typename prepend<T, List>::type;

    template <typename List, typename T>
    struct insert_sorted;

    template <typename T>
    struct insert_sorted<type_list<>, T> {
        using type = type_list<T>;
    };

    template <typename Head, typename... Tail, typename T>
    struct insert_sorted<type_list<Head, Tail...>, T> {
        using type = std::conditional_t<
            type_less_v<T, Head>,
            type_list<T, Head, Tail...>,
            prepend_t<Head, typename insert_sorted<type_list<Tail...>, T>::type>>;
    };

    template <typename List, typename T>
    using insert_sorted_t = typename insert_sorted<List, T>::type;

} // namespace detail

template <typename List>
struct unique_type_list;

template <>
struct unique_type_list<type_list<>> {
    using type = type_list<>;
};

template <typename Head, typename... Tail>
struct unique_type_list<type_list<Head, Tail...>> {
  private:
    using tail = typename unique_type_list<type_list<Tail...>>::type;

  public:
    using type =
        std::conditional_t<type_list_contains_v<Head, tail>, tail, detail::prepend_t<Head, tail>>;
};

template <typename List>
using unique_type_list_t = typename unique_type_list<List>::type;

template <typename List>
struct sort_type_list;

template <>
struct sort_type_list<type_list<>> {
    using type = type_list<>;
};

template <typename Head, typename... Tail>
struct sort_type_list<type_list<Head, Tail...>> {
  private:
    using rest_sorted = typename sort_type_list<type_list<Tail...>>::type;

  public:
    using type = detail::insert_sorted_t<rest_sorted, Head>;
};

template <typename List>
using sort_type_list_t = typename sort_type_list<List>::type;

template <typename List>
struct canonical_type_list {
    using type = sort_type_list_t<unique_type_list_t<List>>;
};

template <typename List>
using canonical_type_list_t = typename canonical_type_list<List>::type;

} // namespace rw::meta