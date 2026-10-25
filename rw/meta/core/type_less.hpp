#pragma once
#include "type_name.hpp"

namespace rw::meta {

template <typename A, typename B>
struct type_less {
  private:
    static consteval bool cmp() {
        const char* a = type_name<A>();
        const char* b = type_name<B>();

        while (*a && (*a == *b)) {
            ++a;
            ++b;
        }
        return *a < *b;
    }

  public:
    static constexpr bool value = cmp();
};

template <typename A, typename B>
constexpr bool type_less_v = type_less<A, B>::value;

} // namespace rw::meta