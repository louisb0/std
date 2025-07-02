#pragma once

#include "initializer_list.hpp"
#include "iterator.hpp"
#include "memory.hpp"

namespace mystd {

template <typename T> class vector {
private:
    T *_start{};
    T *_finish{};
    T *_end_of_storage{};

public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T &;
    using const_reference = const T &;
    using pointer = T *;
    using const_pointer = const T *;
    using iterator = T *;
    using const_iterator = const T *;
    using reverse_iterator = mystd::reverse_iterator<iterator>;
    using const_reverse_iterator = mystd::reverse_iterator<const_iterator>;

    vector() = default;

    explicit vector(size_type count) {
        _start = static_cast<T *>(operator new(sizeof(T) * count));
        _finish = _start;
        _end_of_storage = _start + count;

        try {
            mystd::uninitialized_default_construct(_start, _end_of_storage);
            _finish = _end_of_storage;
        } catch (...) {
            operator delete(_start);
            throw;
        }
    }

    vector(size_type count, const T &value) {
        _start = static_cast<T *>(operator new(sizeof(T) * count));
        _finish = _start;
        _end_of_storage = _start + count;

        try {
            mystd::uninitialized_fill(_start, _end_of_storage, value);
            _finish = _end_of_storage;
        } catch (...) {
            operator delete(_start);
            throw;
        }
    }

    template <input_iterator I> vector(I first, I last) {
        typename iterator_traits<I>::difference_type count = mystd::distance(first, last);

        _start = static_cast<T *>(operator new(sizeof(T) * count));
        _finish = _start;
        _end_of_storage = _start + count;

        try {
            mystd::uninitialized_copy(first, last, _start);
            _finish = _end_of_storage;
        } catch (...) {
            operator delete(_start);
            throw;
        }
    }

    vector(const vector &other) : vector(other.begin(), other.end()) {};
    vector(std::initializer_list<T> il) : vector(mystd::initializer_list<T>(il)) {}
    vector(const mystd::initializer_list<T> &il) : vector(il.begin(), il.end()) {};

    vector(vector &&other) noexcept
        : _start(other._start), _finish(other._finish), _end_of_storage(other._end_of_storage) {
        other._start = nullptr;
        other._finish = nullptr;
        other._end_of_storage = nullptr;
    }

    ~vector() {
        mystd::destroy(begin(), end());
        operator delete(_start);
    }

    // Element access.
    reference operator[](difference_type pos) noexcept { return *(_start + pos); }
    const_reference operator[](difference_type pos) const noexcept { return *(_start + pos); }

    reference front() noexcept { return *_start; }
    const_reference front() const noexcept { return *_start; }

    reference back() noexcept { return *(_finish - 1); }
    const_reference back() const noexcept { return *(_finish - 1); }

    pointer data() noexcept { return _start; }
    const_pointer data() const noexcept { return _start; }

    reference at(size_type pos) {
        if (pos >= size()) {
            throw std::out_of_range("mystd::vector::at was called with an index out of bounds.");
        }
        return *(_start + pos);
    }
    const_reference at(size_type pos) const {
        if (pos >= size()) {
            throw std::out_of_range("mystd::vector::at was called with an index out of bounds.");
        }
        return *(_start + pos);
    }

    // Iterators.
    iterator begin() noexcept { return _start; }
    const_iterator begin() const noexcept { return _start; }
    const_iterator cbegin() const noexcept { return _start; }

    iterator end() noexcept { return _finish; }
    const_iterator end() const noexcept { return _finish; }
    const_iterator cend() const noexcept { return _finish; }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    // Capacity.
    bool empty() const noexcept { return _start == _finish; }
    size_type size() const noexcept { return _finish - _start; }
    size_type max_size() const noexcept { return std::numeric_limits<difference_type>::max(); }
    size_type capacity() const noexcept { return _end_of_storage - _start; }
    // reserve()
    // shrink_to_fit()
};

} // namespace mystd
