#pragma once

#include "algorithm.hpp"
#include "iterator.hpp"
#include "memory.hpp"

#include <algorithm>
#include <cassert>
#include <compare>
#include <utility>

namespace mystd {

template <typename T, typename A = mystd::allocator<T>> class vector {
    [[no_unique_address]] A _allocator{};

    mystd::allocator_traits<A>::pointer _start{};
    mystd::allocator_traits<A>::pointer _finish{};
    mystd::allocator_traits<A>::pointer _end_of_storage{};

public:
    using value_type = T;
    using allocator_type = A;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T &;
    using const_reference = const T &;
    using pointer = mystd::allocator_traits<allocator_type>::pointer;
    using const_pointer = mystd::allocator_traits<allocator_type>::const_pointer;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = mystd::reverse_iterator<iterator>;
    using const_reverse_iterator = mystd::reverse_iterator<const_iterator>;

    // Construction.
    vector() noexcept(noexcept(allocator_type())) : vector(allocator_type()) {}

    explicit vector(const allocator_type &allocator) noexcept : _allocator(allocator) {}

    explicit vector(size_type count, const allocator_type &allocator = allocator_type())
        : _allocator(allocator) {
        _start = _finish = mystd::allocator_traits<allocator_type>::allocate(_allocator, count);
        _end_of_storage = _start + count;

        try {
            mystd::uninitialized_default_construct(_start, _end_of_storage);
            _finish = _end_of_storage;
        } catch (...) {
            mystd::allocator_traits<allocator_type>::deallocate(_allocator, _start, count);
            throw;
        }
    }

    vector(size_type count, const_reference value,
           const allocator_type &allocator = allocator_type())
        : _allocator(allocator) {
        _start = _finish = mystd::allocator_traits<allocator_type>::allocate(_allocator, count);
        _end_of_storage = _start + count;

        try {
            mystd::uninitialized_fill(_start, _end_of_storage, value);
            _finish = _end_of_storage;
        } catch (...) {
            mystd::allocator_traits<allocator_type>::deallocate(_allocator, _start, count);
            throw;
        }
    }

    template <mystd::input_iterator I>
    vector(I first, I last, const allocator_type &allocator = allocator_type())
        : _allocator(allocator) {
        if constexpr (mystd::forward_iterator<I>) {
            auto count = mystd::distance(first, last);

            _start = _finish = mystd::allocator_traits<allocator_type>::allocate(_allocator, count);
            _end_of_storage = _start + count;

            try {
                _finish = mystd::uninitialized_copy(first, last, _start);
            } catch (...) {
                mystd::allocator_traits<allocator_type>::deallocate(_allocator, _start, count);
                throw;
            }
        } else {
            for (; first != last; ++first) {
                push_back(*first);
            }
        }
    }

    vector(const vector &other)
        : vector(other.begin(), other.end(),
                 mystd::allocator_traits<allocator_type>::select_on_container_copy_construction(
                     other._allocator)) {};

    vector(const vector &other, const allocator_type &allocator)
        : vector(other.begin(), other.end(), allocator) {};

    vector(vector &&other) noexcept : _allocator(std::move(other._allocator)) {
        // TODO: Implement std::exchange()
        _start = std::exchange(other._start, nullptr);
        _finish = std::exchange(other._finish, nullptr);
        _end_of_storage = std::exchange(other._end_of_storage, nullptr);
    }

    vector(vector &&other, const allocator_type &allocator) : _allocator(allocator) {
        if (_allocator == other._allocator) {
            _start = std::exchange(other._start, nullptr);
            _finish = std::exchange(other._finish, nullptr);
            _end_of_storage = std::exchange(other._end_of_storage, nullptr);
        } else {
            _start = _finish =
                mystd::allocator_traits<allocator_type>::allocate(_allocator, other.size());
            _end_of_storage = _start + other.size();

            try {
                _finish = mystd::uninitialized_move(other.begin(), other.end(), begin());
            } catch (...) {
                mystd::allocator_traits<allocator_type>::deallocate(_allocator, _start,
                                                                    other.size());
                throw;
            }
        }
    }

    vector(std::initializer_list<value_type> il, const allocator_type &allocator = allocator_type())
        : vector(il.begin(), il.end(), allocator) {};

    ~vector() {
        if (_start) {
            mystd::destroy(begin(), end());
            mystd::allocator_traits<allocator_type>::deallocate(_allocator, _start, capacity());
        }
    }

    vector &operator=(const vector &other) {
        if (this != &other) {
            if constexpr (mystd::allocator_traits<
                              allocator_type>::propagate_on_container_copy_assignment::value) {
                if (_allocator != other._allocator) {
                    if (_start) {
                        mystd::destroy(begin(), end());
                        mystd::allocator_traits<allocator_type>::deallocate(_allocator, _start,
                                                                            capacity());
                    }

                    _start = _finish = _end_of_storage = nullptr;
                    _allocator = other._allocator;
                }
            }

            if (other.size() > capacity()) {
                vector temp(other, _allocator);
                swap(temp);
            } else {
                size_type common_size = std::min(size(), other.size());
                mystd::copy(other.begin(), other.begin() + common_size, begin());

                if (size() > common_size) {
                    mystd::destroy(begin() + common_size, end());
                } else if (other.size() > common_size) {
                    mystd::uninitialized_copy(other.begin() + common_size, other.end(), end());
                }

                _finish = _start + other.size();
            }
        }

        return *this;
    }

    vector &operator=(vector &&other) {
        constexpr bool propagate =
            mystd::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value;

        if (propagate || _allocator == other._allocator) {
            mystd::swap(_allocator, other._allocator);
            mystd::swap(_start, other._start);
            mystd::swap(_finish, other._finish);
            mystd::swap(_end_of_storage, other._end_of_storage);
        } else {
            reserve(other.size());

            if constexpr (std::is_nothrow_move_constructible_v<T> ||
                          !std::is_copy_constructible_v<T>) {
                _finish = mystd::uninitialized_move(other.begin(), other.end(), _start);
            } else {
                _finish = mystd::uninitialized_copy(other.begin(), other.end(), _start);
            }
        }

        return *this;
    }

    vector &operator=(std::initializer_list<value_type> il) {
        *this = vector(il);
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

    allocator_type get_allocator() const noexcept { return _allocator; }

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

        pointer new_start = mystd::allocator_traits<allocator_type>::allocate(_allocator, new_cap);
        pointer new_finish = new_start;

        try {
            if constexpr (std::is_nothrow_move_constructible_v<T> ||
                          !std::is_copy_constructible_v<T>) {
                new_finish = mystd::uninitialized_move(begin(), end(), new_start);
            } else {
                new_finish = mystd::uninitialized_copy(begin(), end(), new_start);
            }

            mystd::destroy(begin(), end());
            if (_start) {
                mystd::allocator_traits<allocator_type>::deallocate(_allocator, _start, capacity());
            }

            _start = new_start;
            _finish = new_finish;
            _end_of_storage = new_start + new_cap;
        } catch (...) {
            mystd::allocator_traits<allocator_type>::deallocate(_allocator, new_start, new_cap);
            throw;
        }
    }

    void shrink_to_fit() {
        if (size() == capacity()) {
            return;
        }

        pointer new_start = mystd::allocator_traits<allocator_type>::allocate(_allocator, size());
        pointer new_finish = new_start;

        try {
            if constexpr (std::is_nothrow_move_constructible_v<value_type> ||
                          !std::is_copy_constructible_v<value_type>) {
                new_finish = mystd::uninitialized_move(begin(), end(), new_start);
            } else {
                new_finish = mystd::uninitialized_copy(begin(), end(), new_start);
            }

            mystd::destroy(begin(), end());
            if (_start) {
                mystd::allocator_traits<allocator_type>::deallocate(_allocator, _start, capacity());
            }

            _start = new_start;
            _finish = new_finish;
            _end_of_storage = new_start + size();
        } catch (...) {
            mystd::allocator_traits<allocator_type>::deallocate(_allocator, new_start, size());
            throw;
        }
    }

    // Modifiers.
    template <typename... Args> iterator emplace(const_iterator cpos, Args &&...args) {
        auto offset = mystd::distance(cbegin(), cpos);
        if (size() == capacity()) {
            reserve(size() == 0 ? 1 : 2 * size());
        }

        iterator pos = begin() + offset;

        mystd::allocator_traits<allocator_type>::construct(_allocator, end(),
                                                           mystd::forward<Args>(args)...);
        std::rotate(pos, end(), end() + 1);

        ++_finish;
        return pos;
    }

    template <typename... Args> reference emplace_back(Args &&...args) {
        return *emplace(cend(), mystd::forward<Args>(args)...);
    }

    void push_back(const_reference value) { emplace_back(value); }
    void push_back(value_type &&value) { emplace_back(std::move(value)); }

    void pop_back() noexcept {
        --_finish;
        mystd::allocator_traits<allocator_type>::destroy(_allocator, _finish);
    }

    iterator insert(const_iterator cpos, const_reference value) { return emplace(cpos, value); }
    iterator insert(const_iterator cpos, value_type &&value) {
        return emplace(cpos, mystd::move(value));
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
        difference_type pos_offset = cpos - cbegin();
        size_type original_size = size();

        if constexpr (mystd::forward_iterator<I>) {
            size_type count = static_cast<size_type>(std::distance(first, last));
            reserve(size() + count);

            mystd::uninitialized_copy(first, last, end());
            _finish += count;
        } else {
            for (; first != last; ++first) {
                push_back(*first);
            }
        }

        iterator pos = begin() + pos_offset;
        std::rotate(pos, begin() + original_size, end());

        return pos;
    }

    iterator insert(const_iterator cpos, std::initializer_list<value_type> il) {
        return insert(cpos, il.begin(), il.end());
    }

    iterator erase(const_iterator cpos) {
        iterator pos = begin() + (cpos - cbegin());

        mystd::move(pos + 1, end(), pos);
        mystd::allocator_traits<allocator_type>::destroy(_allocator, end() - 1);

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

    // NOTE: It is UB to call swap() on containers with different allocators.
    void swap(vector &other) noexcept {
        if constexpr (mystd::allocator_traits<allocator_type>::propagate_on_container_swap::value) {
            mystd::swap(_allocator, other._allocator);
        }

        mystd::swap(_start, other._start);
        mystd::swap(_finish, other._finish);
        mystd::swap(_end_of_storage, other._end_of_storage);
    }
};

template <typename T, typename A>
std::strong_ordering operator<=>(const vector<T, A> &lhs, const vector<T, A> &rhs) {
    size_t min_size = std::min(lhs.size(), rhs.size());

    for (size_t i = 0; i < min_size; i++) {
        auto cmp = lhs[i] <=> rhs[i];
        if (cmp != 0) {
            return cmp;
        }
    }

    return lhs.size() <=> rhs.size();
}

template <typename T, typename A>
bool operator==(const vector<T, A> &lhs, const vector<T, A> &rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }

    return (lhs <=> rhs) == 0;
}

template <typename T, typename A> void swap(vector<T, A> &a, vector<T, A> &b) { a.swap(b); }

} // namespace mystd
