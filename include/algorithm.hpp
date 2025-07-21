#pragma once

#include "bits/iterator_concepts.hpp"
#include "utility.hpp"
#include <concepts>

namespace mystd {

template <mystd::input_iterator I,
          mystd::output_iterator<typename mystd::iterator_traits<I>::value_type &&> O>
O move(I first, I last, O d_first) {
    for (; first != last; ++first, ++d_first) {
        *d_first = mystd::move(*first);
    }

    return d_first;
}

template <mystd::bidirectional_iterator I, mystd::bidirectional_iterator O>
O move_backward(I first, I last, O d_last) {
    while (first != last) {
        *(--d_last) = mystd::move(*--last);
    }

    return d_last;
}

template <mystd::forward_iterator I,
          mystd::output_iterator<typename mystd::iterator_traits<I>::value_type> O>
O copy(I first, I last, O d_first) {
    for (; first != last; ++first, ++d_first) {
        *d_first = *first;
    }

    return d_first;
}

template <mystd::forward_iterator I, typename T> void fill(I first, I last, const T &value) {
    for (; first != last; ++first) {
        *first = value;
    }
}

template <mystd::forward_iterator I1, mystd::forward_iterator I2>
I2 swap_ranges(I1 first, I1 last, I2 d_first) {
    for (; first != last; ++first, ++d_first) {
        mystd::swap(*first, *d_first);
    }

    return d_first;
}

template <mystd::input_iterator I, typename T> I find(I first, I last, const T &value) {
    for (; first != last; ++first) {
        if (*first == value) {
            return first;
        }
    }

    return last;
}

template <mystd::input_iterator I, std::predicate<typename mystd::iterator_traits<I>::value_type> P>
I find_if(I first, I last, P p) {
    for (; first != last; ++first) {
        if (p(*first)) {
            return first;
        }
    }

    return last;
}

template <mystd::input_iterator I, std::predicate<typename mystd::iterator_traits<I>::value_type> P>
I find_if_not(I first, I last, P p) {
    for (; first != last; ++first) {
        if (!p(*first)) {
            return first;
        }
    }

    return last;
}

} // namespace mystd
