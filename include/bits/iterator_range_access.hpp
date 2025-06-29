#pragma once

#include "bits/iterator_reverse.hpp"

namespace mystd {

template <typename T> struct initializer_list;

template <typename T> bool empty(const initializer_list<T> &il) { return il.size() == 0; }
template <typename T> const T *data(const initializer_list<T> &il) { return il.begin(); }

template <typename T> reverse_iterator<const T *> rbegin(const initializer_list<T> &il) {
    return reverse_iterator(il.end());
};
template <typename T> reverse_iterator<const T *> crbegin(const initializer_list<T> &il) {
    return reverse_iterator(il.end());
};

template <typename T> reverse_iterator<const T *> rend(const initializer_list<T> &il) {
    return reverse_iterator(il.begin());
};
template <typename T> reverse_iterator<const T *> crend(const initializer_list<T> &il) {
    return reverse_iterator(il.begin());
};

} // namespace mystd
