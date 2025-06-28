#pragma once

#include "iterator.hpp"

#include <memory>

namespace mystd {

template <input_iterator InputIt, forward_iterator OutputIt>
OutputIt uninitialized_copy(InputIt first, InputIt last, OutputIt result) {
    using T = iterator_traits<OutputIt>::value_type;

    OutputIt current = result;
    try {
        for (; first != last; ++first, ++current)
            ::new (static_cast<void *>(std::addressof(*current))) T(*first);
        return current;
    } catch (...) {
        for (; result != current; ++result)
            (*result).~T();
        throw;
    }
}

} // namespace mystd
