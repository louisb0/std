#pragma once

#include <concepts>
#include <iterator>
#include <type_traits>
#include <utility>

namespace mystd {

// TODO & NOTE:
//  - implement std::forward()
//  - implement std::indirectly_readable() and std::indirectly_writable()

template <typename T>
concept can_reference =
    std::is_reference_v<T> || !std::is_same_v<T, std::add_lvalue_reference_t<T>>;

struct input_iterator_tag {};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};

template <typename I, typename T = void> struct iterator_tag {};
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
    using value_type = I::value_type;
};

template <typename T> struct iterator_traits<T *> {
    using value_type = T;
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
 * - TODO
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

} // namespace mystd
