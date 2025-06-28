#pragma once

#include <concepts>
#include <iterator>
#include <type_traits>
#include <utility>

namespace mystd {

// TODO:
//  - implement std::forward()

template <typename T>
concept can_reference =
    std::is_reference_v<T> || !std::is_same_v<T, std::add_lvalue_reference_t<T>>;

struct input_iterator_tag {};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};

template <typename I, typename = void> struct iterator_tag {};
template <typename I> struct iterator_tag<I, std::void_t<typename I::iterator_category>> {
    using type = I::iterator_category;
};
template <typename T> struct iterator_tag<T *> {
    using type = mystd::random_access_iterator_tag;
};

template <typename I, typename Tag>
concept matches_iterator_tag = requires { typename iterator_tag<I>::type; } &&
                               std::derived_from<typename iterator_tag<I>::type, Tag>;

template <typename I> struct iterator_traits {
    using difference_type = I::difference_type;
    using value_type = I::value_type;
    using pointer = I::pointer;
    using reference = I::reference;
    using iterator_category = I::iterator_category;
};
template <typename T> struct iterator_traits<T *> {
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = random_access_iterator_tag;
};
template <typename T> struct iterator_traits<const T *> {
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = const T *;
    using reference = const T &;
    using iterator_category = random_access_iterator_tag;
};

// clang-format off

/**
 * [semantics]:
 * - for any iterator i, either both ++i and i++ are valid or both are invalid
 * - when valid, both ++i and i++ advance to the same next iterator state
 * - when valid, ++i returns a reference to the same object i
 */
template <typename I>
concept weakly_incrementable =
    std::movable<I> &&
    requires(I i) {
        { ++i } -> std::same_as<I &>;
        i++;
    };

/**
 * [semantics]:
 * - for incrementable a and b:
 *      - post-increment returns the original object, (a == b) implies (a++ == b)
 *      - post-increment preserves equality, (a == b) implies ((void)a++, a) == ++b)
 */
template <typename I>
concept incrementable =
    weakly_incrementable<I> &&
    std::regular<I> &&
    requires(I i) {
        { i++ } -> std::same_as<I>;
    };

/**
 * [semantics]
 * - equality preserving, *i == *i
 */
template <typename I>
concept input_or_output_iterator =
    weakly_incrementable<I> &&
    requires(I i) { *i; } &&
    can_reference<decltype(*std::declval<I>())>;

/**
 * [semantics]
 * - sensible post-increment, i.e. *i++ = t is equivalent to *i = t; ++i;
 */
template <typename I, typename T>
concept output_iterator =
    input_or_output_iterator<I> &&
    std::indirectly_writable<I, T> &&
    requires (I i, T&& t) {
        *i++ = std::forward<T>(t);
    };

/**
 * [semantics]
 *  - none
 */
template <typename I>
concept input_iterator =
    input_or_output_iterator<I> &&
    std::indirectly_readable<I> &&
    std::equality_comparable<I> &&
    matches_iterator_tag<I, input_iterator_tag>;

/**
 * [semantics]
 *  - for i and j, comparison is defined only over the same sequence, or if value-initialized
 *  - multipass guarantee (i.e. copies are independent, as per incrementable<T>'s copy semantics)
 *      - if i == j, then ++i == ++j
 *      - *i == ((void)[](auto x){ ++x; }(i), *i)
 */
template <typename I>
concept forward_iterator =
    input_iterator<I> &&
    incrementable<I> &&
    matches_iterator_tag<I, forward_iterator_tag>;

/**
 * [semantics]
 * - an iterator i is valid if there is some ++s = i
 * - for any iterator i, either both --i and i-- are valid or both are invalid
 * - when valid:
 *     -  --i returns a reference to the same object i: addressof(--a) == addressof(a)
 *     -  i-- returns a copy of the object: if (a == b) then (a-- == b):
 *     - increment and decrement are inverses: if (a == b) then (++(--a) == b)
 *     - operators alter state equivalently: if (a == b), then after a-- and --b , still (a == b)
 */
template <typename I>
concept bidirectional_iterator =
    forward_iterator<I> &&
    requires(I i) {
        { --i } -> std::same_as<I &>;
        { i-- } -> std::same_as<I>;
    } &&
    matches_iterator_tag<I, bidirectional_iterator_tag>;

/**
 * [semantics]
 * - all operations are constant time
 * - for iterators a, b where b is reachable from a, and n = b - a:
 *     - (a += n) == b and returns reference to a
 *     - (a + n) == (a += n) == (n + a)
 *     - addition is associative: a + (x + y) == (a + x) + y
 *     - a + 0 == a
 *     - subtraction inverts addition: (b -= n) == (b + (-n)) == a and returns reference to b
 *     - if b is dereferenceable, then a[n] == *b
 *     - a <= b is true
 */
template <typename I>
concept random_access_iterator =
    bidirectional_iterator<I> &&
    std::totally_ordered<I> &&
    std::sized_sentinel_for<I, I> &&
    requires(I i, const I j, const iterator_traits<I>::difference_type n) {
        { i += n } -> std::same_as<I &>;
        { i -= n } -> std::same_as<I &>;
        { j + n } -> std::same_as<I>;
        { j - n } -> std::same_as<I>;
        { j[n] } -> std::same_as<typename iterator_traits<I>::reference>;
    } &&
    matches_iterator_tag<I, random_access_iterator_tag>;

// clang-format on

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
bool operator==(const reverse_iterator<I1> &lhs, const reverse_iterator<I2> &rhs) {
    return lhs.base() == rhs.base();
}

template <typename I1, typename I2>
bool operator!=(const reverse_iterator<I1> &lhs, const reverse_iterator<I2> &rhs) {
    return lhs.base() != rhs.base();
}

template <typename I1, typename I2>
bool operator<(const reverse_iterator<I1> &lhs, const reverse_iterator<I2> &rhs) {
    return lhs.base() > rhs.base();
}

template <typename I1, typename I2>
bool operator<=(const reverse_iterator<I1> &lhs, const reverse_iterator<I2> &rhs) {
    return lhs.base() >= rhs.base();
}

template <typename I1, typename I2>
bool operator>(const reverse_iterator<I1> &lhs, const reverse_iterator<I2> &rhs) {
    return lhs.base() < rhs.base();
}

template <typename I1, typename I2>
bool operator>=(const reverse_iterator<I1> &lhs, const reverse_iterator<I2> &rhs) {
    return lhs.base() <= rhs.base();
}

template <typename I>
reverse_iterator<I> operator+(typename reverse_iterator<I>::difference_type n,
                              const reverse_iterator<I> &it) {
    return it.operator+(n);
}

template <typename I1, typename I2>
auto operator-(const reverse_iterator<I1> &lhs, const reverse_iterator<I2> &rhs)
    -> decltype(rhs.base() - lhs.base()) {
    return rhs.base() - lhs.base();
}

template <typename T> class initializer_list;

template <typename T> bool empty(const initializer_list<T> &il) { return il.size() == 0; }

template <typename T> const T *data(const initializer_list<T> &il) { return il.begin(); }

template <typename T> reverse_iterator<const T *> rbegin(const initializer_list<T> &il) {
    return reverse_iterator(il.end());
};

template <typename T> reverse_iterator<const T *> crbegin(const initializer_list<T> &il) {
    return reverse_iterator(il.end());
};

template <typename T> reverse_iterator<const T *> rend(const initializer_list<T> &il) {
    return reverse_iterator(il.begin());
};

template <typename T> reverse_iterator<const T *> crend(const initializer_list<T> &il) {
    return reverse_iterator(il.begin());
};

} // namespace mystd
