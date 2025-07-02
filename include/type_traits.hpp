#pragma once

#include <type_traits>

namespace mystd {

template <typename T, typename U> struct is_same : std::false_type {};
template <typename T> struct is_same<T, T> : std::true_type {};
template <typename T, typename U> constexpr inline bool is_same_v = is_same<T, U>::value;

template <typename T> struct is_reference : std::false_type {};
template <typename T> struct is_reference<T &> : std::true_type {};
template <typename T> struct is_reference<T &&> : std::true_type {};
template <typename T> constexpr inline bool is_reference_v = is_reference<T>::value;

template <typename T>
struct is_referenceable
    : std::bool_constant<is_reference_v<T> || !is_same_v<T, std::add_lvalue_reference_t<T>>> {};
template <typename T> constexpr inline bool is_referenceable_v = is_referenceable<T>::value;

template <typename T> struct remove_reference {
    using type = T;
};
template <typename T> struct remove_reference<T &> {
    using type = T;
};
template <typename T> struct remove_reference<T &&> {
    using type = T;
};
template <typename T> using remove_reference_t = remove_reference<T>::type;

} // namespace mystd
