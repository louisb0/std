#pragma once

#include "bits/iterator_base_types.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

#include <concepts>
#include <iterator>

namespace mystd {

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
    mystd::is_referenceable_v<decltype(*std::declval<I>())>;

/**
 * [semantics]
 * - sensible post-increment, i.e. *i++ = t is equivalent to *i = t; ++i;
 */
template <typename I, typename T>
concept output_iterator =
    input_or_output_iterator<I> &&
    std::indirectly_writable<I, T> &&
    requires (I i, T&& t) {
        *i++ = mystd::forward<T>(t);
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
    std::derived_from<typename mystd::iterator_traits<I>::iterator_category, mystd::input_iterator_tag>;

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
    std::derived_from<typename mystd::iterator_traits<I>::iterator_category, mystd::forward_iterator_tag>;

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
    std::derived_from<typename mystd::iterator_traits<I>::iterator_category, mystd::bidirectional_iterator_tag>;

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
    requires(I i, const I j, const mystd::iterator_traits<I>::difference_type n) {
        { i += n } -> std::same_as<I &>;
        { i -= n } -> std::same_as<I &>;
        { j + n } -> std::same_as<I>;
        { j - n } -> std::same_as<I>;
        { j[n] } -> std::same_as<typename mystd::iterator_traits<I>::reference>;
    } &&
    std::derived_from<typename mystd::iterator_traits<I>::iterator_category, mystd::random_access_iterator_tag>;

// clang-format on

} // namespace mystd
