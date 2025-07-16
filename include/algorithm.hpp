#pragma once

#include "iterator.hpp"
#include "utility.hpp"

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

} // namespace mystd
