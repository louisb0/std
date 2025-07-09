#pragma once

#include "iterator.hpp"

namespace mystd {

template <input_iterator I, output_iterator<typename iterator_traits<I>::value_type &&> O>
O move(I first, I last, O d_first) {
    for (; first != last; ++first, ++d_first) {
        *d_first = std::move(*first);
    }

    return d_first;
}

template <bidirectional_iterator I, bidirectional_iterator O>
O move_backward(I first, I last, O d_last) {
    while (first != last) {
        *(--d_last) = std::move(*--last);
    }

    return d_last;
}

template <forward_iterator I, output_iterator<typename iterator_traits<I>::value_type> O>
O copy(I first, I last, O d_first) {
    for (; first != last; ++first, ++d_first) {
        *d_first = *first;
    }

    return d_first;
}

} // namespace mystd
