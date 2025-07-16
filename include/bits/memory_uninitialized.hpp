#pragma once

#include "bits/iterator_base_types.hpp"
#include "bits/iterator_concepts.hpp"

namespace mystd {

template <mystd::forward_iterator I> void destroy(I first, I last) {
    using T = typename mystd::iterator_traits<I>::value_type;
    static_assert(std::is_destructible_v<T>);

    for (; first != last; ++first) {
        (*first).~T();
    }
}

template <mystd::input_iterator I, mystd::forward_iterator O>
O uninitialized_copy(I first, I last, O result) {
    using T = typename mystd::iterator_traits<O>::value_type;
    using U = typename mystd::iterator_traits<I>::value_type;
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

template <mystd::input_iterator I, mystd::forward_iterator O>
O uninitialized_move(I first, I last, O result) {
    using T = typename mystd::iterator_traits<O>::value_type;
    using U = typename mystd::iterator_traits<I>::value_type;
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

template <mystd::forward_iterator I> void uninitialized_default_construct(I first, I last) {
    using T = typename mystd::iterator_traits<I>::value_type;

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

template <mystd::forward_iterator I, typename T>
void uninitialized_fill(I first, I last, const T &value) {
    using V = typename mystd::iterator_traits<I>::value_type;
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
