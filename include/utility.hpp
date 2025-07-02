#pragma once

#include <type_traits.hpp>

namespace mystd {

template <typename T> T &&forward(mystd::remove_reference_t<T> &arg) {
    return static_cast<T &&>(arg);
}

template <typename T> T &&forward(mystd::remove_reference_t<T> &&arg) {
    return static_cast<T &&>(arg);
}

template <typename T> mystd::remove_reference_t<T> &&move(T &&arg) {
    return static_cast<typename mystd::remove_reference_t<T> &&>(arg);
}

} // namespace mystd
