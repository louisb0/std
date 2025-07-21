#pragma once

#include "bits/iterator_base_types.hpp"
#include "bits/iterator_concepts.hpp"

#include <concepts>

namespace mystd {

template <mystd::input_iterator I>
typename mystd::iterator_traits<I>::difference_type distance(I first, I last) {
    if constexpr (std::derived_from<typename mystd::iterator_traits<I>::iterator_category,
                                    mystd::random_access_iterator_tag>) {
        return last - first;
    } else {
        typename mystd::iterator_traits<I>::difference_type result{};
        while (first != last) {
            ++first;
            ++result;
        }
        return result;
    }
}

template <mystd::input_iterator I>
void advance(I &it, typename mystd::iterator_traits<I>::difference_type n) {
    using category = typename mystd::iterator_traits<I>::iterator_category;

    if constexpr (std::derived_from<category, mystd::random_access_iterator_tag>) {
        it += n;
    } else {
        while (n > 0) {
            --n;
            ++it;
        }

        if constexpr (std::derived_from<category, mystd::bidirectional_iterator_tag>) {
            while (n < 0) {
                ++n;
                --it;
            }
        }
    }
}

template <mystd::input_iterator I>
I next(I it, typename mystd::iterator_traits<I>::difference_type n = 1) {
    advance(it, 1);
    return it;
}

} // namespace mystd
