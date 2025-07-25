#pragma once

#include "type_traits.hpp"

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

template <typename T>
void swap(T &a, T &b) noexcept(std::is_nothrow_move_constructible<T>::value &&
                               std::is_nothrow_move_assignable<T>::value) {
    T tmp = mystd::move(a);
    a = mystd::move(b);
    b = mystd::move(tmp);
}

template <typename T, typename U>
T exchange(T &obj, U &&new_value) noexcept(std::is_nothrow_move_constructible<T>::value &&
                                           std::is_nothrow_assignable<T &, U>::value) {
    T old_value = obj;
    obj = mystd::forward<U>(new_value);
    return old_value;
}

} // namespace mystd
