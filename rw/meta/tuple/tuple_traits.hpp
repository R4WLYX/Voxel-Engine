#pragma once
#include <tuple>

#include "../list/list_traits.hpp"

namespace rw::meta {

template <typename T, typename... Ts>
struct index_of<T, std::tuple<Ts...>> : index_of<T, type_list<Ts...>> {};

} // namespace rw::meta