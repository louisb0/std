#pragma once

#include "algorithm.hpp"
#include "iterator.hpp"
#include "memory.hpp"

#include <algorithm>
#include <compare>

namespace mystd {

/*

- operator=
- add allocator
- project wide refactor (includes, consistent mystd::, etc.)

*/

template <typename T> class vector {
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

    // Construction.
    vector() = default;

    explicit vector(size_type count) {
        _start = static_cast<value_type *>(operator new(sizeof(value_type) * count));
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

    vector(size_type count, const_reference value) {
        _start = static_cast<value_type *>(operator new(sizeof(value_type) * count));
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

    template <mystd::input_iterator I> vector(I first, I last) {
        typename mystd::iterator_traits<I>::difference_type count = mystd::distance(first, last);

        _start = static_cast<value_type *>(operator new(sizeof(value_type) * count));
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
    vector(std::initializer_list<value_type> il) : vector(il.begin(), il.end()) {};

    vector(const vector &other) : vector(other.begin(), other.end()) {};
    vector(vector &&other) noexcept { swap(other); }

    ~vector() noexcept {
        mystd::destroy(begin(), end());
        operator delete(_start);
    }

    vector &operator=(const vector &other) {
        if (this == &other) {
            return *this;
        }

        if (other.size() > capacity()) {
            vector temp(other);
            swap(temp);
        } else {
            // TODO: Implement copy().
            size_t common_size = std::min(size(), other.size());
            std::copy(other.begin(), other.begin() + common_size, begin());

            if (size() > common_size) {
                mystd::destroy(begin() + common_size, end());
            } else if (other.size() > common_size) {
                mystd::uninitialized_copy(other.begin() + common_size, other.end(), end());
            }

            _finish = _start + other.size();
        }

        return *this;
    }

    vector &operator=(std::initializer_list<value_type> il) noexcept {
        *this = vector(il);
        return *this;
    }

    vector &operator=(vector &&other) noexcept {
        swap(other);
        return *this;
    }

    // Access.
    reference operator[](size_type pos) noexcept { return *(_start + pos); }
    const_reference operator[](size_type pos) const noexcept { return *(_start + pos); }

    reference front() noexcept { return *_start; }
    const_reference front() const noexcept { return *_start; }

    reference back() noexcept { return *(_finish - 1); }
    const_reference back() const noexcept { return *(_finish - 1); }

    pointer data() noexcept { return _start; }
    const_pointer data() const noexcept { return _start; }

    reference at(size_type pos) {
        if (pos >= size()) {
            throw std::out_of_range("mystd::vector::at() was called with an index out of bounds.");
        }
        return *(_start + pos);
    }
    const_reference at(size_type pos) const {
        if (pos >= size()) {
            throw std::out_of_range("mystd::vector::at() was called with an index out of bounds.");
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

    void reserve(size_type new_cap) {
        if (new_cap <= capacity()) {
            return;
        }

        if (new_cap > max_size()) {
            throw std::length_error(
                "mystd::vector::reserve() was called with too large a capacity.");
        }

        value_type *new_start =
            static_cast<value_type *>(operator new(sizeof(value_type) * new_cap));
        value_type *new_finish = new_start;

        try {
            if constexpr (std::is_nothrow_move_constructible_v<T> ||
                          !std::is_copy_constructible_v<T>) {
                new_finish = mystd::uninitialized_move(begin(), end(), new_start);
            } else {
                new_finish = mystd::uninitialized_copy(begin(), end(), new_start);
            }

            mystd::destroy(begin(), end());
            operator delete(_start);

            _start = new_start;
            _finish = new_finish;
            _end_of_storage = new_start + new_cap;
        } catch (...) {
            operator delete(new_start);
            throw;
        }
    }

    void shrink_to_fit() {
        if (size() == capacity()) {
            return;
        }

        value_type *new_start =
            static_cast<value_type *>(operator new(sizeof(value_type) * size()));
        value_type *new_finish = new_start;

        try {
            if constexpr (std::is_nothrow_move_constructible_v<T> ||
                          !std::is_copy_constructible_v<T>) {
                new_finish = mystd::uninitialized_move(begin(), end(), new_start);
            } else {
                new_finish = mystd::uninitialized_copy(begin(), end(), new_start);
            }

            mystd::destroy(begin(), end());
            operator delete(_start);

            _start = new_start;
            _finish = new_finish;
            _end_of_storage = new_start + size();
        } catch (...) {
            operator delete(new_start);
            throw;
        }
    }

    // Modifiers.
    template <typename... Args> iterator emplace(const_iterator cpos, Args &&...args) {
        difference_type offset = mystd::distance(cbegin(), cpos);
        if (size() == capacity()) {
            reserve(size() == 0 ? 1 : 2 * size());
        }

        iterator pos = begin() + offset;

        new (end()) value_type(mystd::forward<Args>(args)...);
        std::rotate(pos, end(), end() + 1); // NOTE: Consider implementing.

        ++_finish;
        return pos;
    }

    template <typename... Args> reference emplace_back(Args &&...args) {
        if (size() == capacity()) {
            reserve(size() == 0 ? 1 : 2 * size());
        }

        new (_finish) value_type(mystd::forward<Args>(args)...);
        return *(_finish++);
    }

    void push_back(const_reference value) { emplace_back(value); }
    void push_back(value_type &&value) { emplace_back(std::move(value)); }
    void pop_back() noexcept { (--_finish)->~value_type(); }

    iterator insert(const_iterator cpos, const_reference value) { return emplace(cpos, value); }
    iterator insert(const_iterator cpos, value_type &&value) {
        return emplace(cpos, std::move(value));
    }

    iterator insert(const_iterator cpos, size_type count, const_reference value) {
        difference_type pos_offset = cpos - cbegin();
        reserve(size() + count);

        iterator pos = begin() + pos_offset;

        mystd::uninitialized_fill(end(), end() + count, value);
        std::rotate(pos, end(), end() + count);

        _finish += count;
        return pos;
    }

    template <mystd::input_iterator I> iterator insert(const_iterator cpos, I first, I last) {
        size_type count = static_cast<size_type>(std::distance(first, last));
        difference_type pos_offset = cpos - cbegin();
        reserve(size() + count);

        iterator pos = begin() + pos_offset;

        mystd::uninitialized_copy(first, last, end());
        std::rotate(pos, end(), end() + count);

        _finish += count;
        return pos;
    }

    iterator insert(const_iterator cpos, std::initializer_list<value_type> il) {
        return insert(cpos, il.begin(), il.end());
    }

    iterator erase(const_iterator cpos) {
        iterator pos = begin() + (cpos - cbegin());

        mystd::move(pos + 1, end(), pos);
        (end() - 1)->~value_type();

        --_finish;
        return pos;
    }

    iterator erase(const_iterator cfirst, const_iterator clast) {
        iterator first = begin() + (cfirst - cbegin());
        iterator last = begin() + (clast - cbegin());

        auto new_end = mystd::move(last, end(), first);
        mystd::destroy(new_end, end());

        _finish = new_end;
        return first;
    }

    void clear() {
        mystd::destroy(begin(), end());
        _finish = _start;
    }

    void resize(size_type count, const_reference value) {
        iterator new_end = _start + count;

        if (count < size()) {
            mystd::destroy(new_end, end());
        } else if (count > size()) {
            reserve(count);

            new_end = _start + count;
            mystd::uninitialized_fill(end(), new_end, value);
        }

        _finish = new_end;
    }
    void resize(size_type count) { resize(count, value_type{}); }

    void swap(vector &other) noexcept {
        mystd::swap(_start, other._start);
        mystd::swap(_finish, other._finish);
        mystd::swap(_end_of_storage, other._end_of_storage);
    }
};

template <class T> std::strong_ordering operator<=>(const vector<T> &lhs, const vector<T> &rhs) {
    size_t min_size = std::min(lhs.size(), rhs.size());

    for (size_t i = 0; i < min_size; i++) {
        auto cmp = lhs[i] <=> rhs[i];
        if (cmp != 0) {
            return cmp;
        }
    }

    return lhs.size() <=> rhs.size();
}

template <class T> bool operator==(const vector<T> &lhs, const vector<T> &rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }

    return (lhs <=> rhs) == 0;
}

} // namespace mystd
