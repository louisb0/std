#pragma once

#include "memory.hpp"

#include <cstddef>
#include <cstring>

#include <initializer_list>
#include <stdexcept>

namespace mystd {

template <typename T> class initializer_list {
private:
    T _data[16];
    size_t _size;

public:
    using size_type = size_t;
    using value_type = T;
    using reference = const T &;
    using const_reference = const T &;
    using iterator = const T *;
    using const_iterator = const T *;

    initializer_list(std::initializer_list<T> il) : _size(il.size()) {
        if (il.size() > 16) {
            throw std::length_error("mystd::initializer_list accepts at most 16 elements.");
        }

        mystd::uninitialized_copy(il.begin(), il.end(), _data);
    }

    size_type size() const noexcept { return _size; }

    iterator begin() const noexcept { return _data; }
    iterator end() const noexcept { return _data + _size; }
};

template <typename E> const E *begin(const initializer_list<E> &il) { return il.begin(); }
template <typename E> const E *end(const initializer_list<E> &il) noexcept { return il.end(); }

} // namespace mystd
