#pragma once

#include "algorithm.hpp"
#include "bits/iterator_reverse.hpp"

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <limits>
#include <stdexcept>

namespace mystd {

template <typename T, std::size_t N> class array {
    T _data[N]{};

public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T &;
    using const_reference = const T &;
    using pointer = T *;
    using const_pointer = const T *;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = mystd::reverse_iterator<iterator>;
    using const_reverse_iterator = mystd::reverse_iterator<const_iterator>;

    // Construction.
    array() = default;
    array(std::initializer_list<T> il) { mystd::copy(il.begin(), il.end(), _data); }
    array &operator=(std::initializer_list<T> il) {
        mystd::copy(il.begin(), il.end(), _data);
        return *this;
    }

    // Access.
    reference operator[](size_type pos) noexcept { return _data[pos]; }
    const_reference operator[](size_type pos) const noexcept { return _data[pos]; }

    reference front() noexcept { return _data[0]; }
    const_reference front() const noexcept { return _data[0]; }

    reference back() noexcept { return _data[N - 1]; }
    const_reference back() const noexcept { return _data[N - 1]; }

    pointer data() noexcept { return _data; }
    const_pointer data() const noexcept { return _data; }

    reference at(size_type pos) {
        if (pos >= N) {
            throw std::out_of_range("mystd::array::at() was called with an index out of bounds.");
        }

        return _data[pos];
    }
    const_reference at(size_type pos) const {
        if (pos >= N) {
            throw std::out_of_range("mystd::array::at() was called with an index out of bounds.");
        }

        return _data[pos];
    }

    // Iterators.
    iterator begin() noexcept { return _data; }
    const_iterator begin() const noexcept { return _data; }
    const_iterator cbegin() const noexcept { return _data; }

    iterator end() noexcept { return _data + N; }
    const_iterator end() const noexcept { return _data + N; }
    const_iterator cend() const noexcept { return _data + N; }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    // Capacity.
    bool empty() const noexcept { return N == 0; }
    size_type size() const noexcept { return N; }
    size_type max_size() const noexcept { return std::numeric_limits<size_type>::max(); }

    // Modifiers.
    void fill(const_reference value) { mystd::fill(begin(), end(), value); }
    void swap(array &other) { mystd::swap_ranges(other.begin(), other.end(), begin()); }
};

template <typename T, std::size_t N>
auto operator<=>(const array<T, N> &lhs, const array<T, N> &rhs) {
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, std::size_t N>
bool operator==(const array<T, N> &lhs, const array<T, N> &rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T, std::size_t N> void swap(array<T, N> &a, array<T, N> &b) { a.swap(b); }

} // namespace mystd
