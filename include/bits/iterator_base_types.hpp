#pragma once

#include <cstddef>

namespace mystd {

struct input_iterator_tag {};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};

template <typename Category, typename T, typename Distance = std::ptrdiff_t, typename Pointer = T *,
          typename Reference = T &>
struct iterator {
    using iterator_category = Category;
    using value_type = T;
    using pointer = Pointer;
    using reference = Reference;
    using difference_type = Distance;
};

template <typename I> struct iterator_traits {
    using iterator_category = typename I::iterator_category;
    using value_type = typename I::value_type;
    using pointer = typename I::pointer;
    using reference = typename I::reference;
    using difference_type = typename I::difference_type;
};

template <typename T> struct iterator_traits<T *> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using difference_type = std::ptrdiff_t;
};

template <typename T> struct iterator_traits<const T *> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using pointer = const T *;
    using reference = const T &;
    using difference_type = std::ptrdiff_t;
};

} // namespace mystd
