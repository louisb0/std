#pragma once

#include "iterator.hpp"

namespace mystd {

template <input_iterator I, output_iterator<typename iterator_traits<I>::value_type &&> O>
O move(I first, I last, O result) {
    for (; first != last; ++first, ++result) {
        *result = std::move(*first);
    }

    return result;
}

} // namespace mystd
