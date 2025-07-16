#pragma once

#include "bits/iterator_concepts.hpp"

namespace mystd {

template <bidirectional_iterator I> class reverse_iterator {
    I _current{I()};

public:
    using iterator_type = I;

    using difference_type = iterator_traits<I>::difference_type;
    using value_type = iterator_traits<I>::value_type;
    using pointer = iterator_traits<I>::pointer;
    using reference = iterator_traits<I>::reference;
    using iterator_category = iterator_traits<I>::iterator_category;

    reverse_iterator() = default;

    explicit reverse_iterator(iterator_type iter) : _current(iter) {}

    template <typename U>
        requires(!std::is_same_v<U, I> && std::convertible_to<U, I>)
    reverse_iterator(const reverse_iterator<U> &other) : _current(other.base()) {}

    iterator_type base() const noexcept { return _current; }

    reference operator*() const noexcept {
        iterator_type tmp = _current;
        return *--tmp;
    }

    pointer operator->() const noexcept { return std::addressof(operator*()); }

    reference operator[](difference_type n) const noexcept
        requires random_access_iterator<I>
    {
        return _current[-1 - n];
    }

    reverse_iterator &operator++() noexcept {
        --_current;
        return *this;
    }

    reverse_iterator &operator--() noexcept {
        ++_current;
        return *this;
    }

    reverse_iterator operator++(int) noexcept {
        reverse_iterator tmp = *this;
        --_current;
        return tmp;
    }

    reverse_iterator operator--(int) noexcept {
        reverse_iterator tmp = *this;
        _current++;
        return tmp;
    }

    reverse_iterator operator+(difference_type n) const noexcept
        requires random_access_iterator<I>
    {
        return reverse_iterator(_current - n);
    }

    reverse_iterator operator-(difference_type n) const noexcept
        requires random_access_iterator<I>
    {
        return reverse_iterator(_current + n);
    }

    reverse_iterator &operator+=(difference_type n) noexcept
        requires random_access_iterator<I>
    {
        _current -= n;
        return *this;
    }

    reverse_iterator &operator-=(difference_type n) noexcept
        requires random_access_iterator<I>
    {
        _current += n;
        return *this;
    }
};

template <typename I1, typename I2>
auto operator<=>(const reverse_iterator<I1> &lhs, const reverse_iterator<I2> &rhs)
    -> decltype(rhs.base() <=> lhs.base()) {
    return rhs.base() <=> lhs.base();
}

template <typename I1, typename I2>
bool operator==(const reverse_iterator<I1> &lhs, const reverse_iterator<I2> &rhs) {
    return lhs.base() == rhs.base();
}

template <typename I1, typename I2>
auto operator-(const reverse_iterator<I1> &lhs, const reverse_iterator<I2> &rhs)
    -> decltype(rhs.base() - lhs.base()) {
    return rhs.base() - lhs.base();
}

template <typename I>
reverse_iterator<I> operator+(typename reverse_iterator<I>::difference_type n,
                              const reverse_iterator<I> &it) {
    return it.operator+(n);
}

} // namespace mystd
