#pragma once

#include "iterator.hpp"

#include <memory>

namespace mystd {

template <forward_iterator I> void destroy(I first, I last) {
    using T = typename iterator_traits<I>::value_type;
    static_assert(std::is_destructible_v<T>);

    for (; first != last; ++first) {
        (*first).~T();
    }
}

template <input_iterator I, forward_iterator O> O uninitialized_copy(I first, I last, O result) {
    using T = typename iterator_traits<O>::value_type;
    using U = typename iterator_traits<I>::value_type;
    static_assert(std::is_constructible_v<T, const U &>);

    O current = result;
    try {
        for (; first != last; ++first, ++current) {
            ::new (static_cast<void *>(std::addressof(*current))) T(*first);
        }
        return current;
    } catch (...) {
        destroy(result, current);
        throw;
    }
}

template <input_iterator I, forward_iterator O> O uninitialized_move(I first, I last, O result) {
    using T = typename iterator_traits<O>::value_type;
    using U = typename iterator_traits<I>::value_type;
    static_assert(std::is_constructible_v<T, U &&>);

    O current = result;
    try {
        for (; first != last; ++first, ++current) {
            ::new (static_cast<void *>(std::addressof(*current))) T(mystd::move(*first));
        }
        return current;
    } catch (...) {
        destroy(result, current);
        throw;
    }
}

template <forward_iterator I> void uninitialized_default_construct(I first, I last) {
    using T = typename iterator_traits<I>::value_type;
    static_assert(std::is_default_constructible_v<T>);

    I current = first;
    try {
        for (; current != last; current++) {
            ::new (static_cast<void *>(std::addressof(*current))) T();
        }
    } catch (...) {
        destroy(first, current);
        throw;
    }
}

template <forward_iterator I, typename T> void uninitialized_fill(I first, I last, const T &value) {
    using V = typename iterator_traits<I>::value_type;
    static_assert(std::is_constructible_v<V, const T &>);

    I current = first;
    try {
        for (; current != last; current++) {
            ::new (static_cast<void *>(std::addressof(*current))) V(value);
        }
    } catch (...) {
        destroy(first, current);
        throw;
    }
}

} // namespace mystd
