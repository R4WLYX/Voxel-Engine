#pragma once

namespace rw::meta {

template <typename T>
consteval const char* type_name() {
#ifdef __clang__
    return __PRETTY_FUNCTION__;
#elif defined(__GNUC__)
    return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
    return __FUNCSIG__;
#endif
}

} // namespace rw::meta