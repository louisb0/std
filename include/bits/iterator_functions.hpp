#pragma once

#include "bits/iterator_base_types.hpp"
#include "bits/iterator_concepts.hpp"

#include <concepts>

namespace mystd {

template <input_iterator I> typename iterator_traits<I>::difference_type distance(I first, I last) {
    if constexpr (std::derived_from<typename iterator_traits<I>::iterator_category,
                                    random_access_iterator_tag>) {
        return last - first;
    } else {
        typename iterator_traits<I>::difference_type result{};
        while (first != last) {
            ++first;
            ++result;
        }
        return result;
    }
}

} // namespace mystd
