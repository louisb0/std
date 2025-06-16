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

template <typename I, typename T = void> struct iter_tag {};
template <typename I> struct iter_tag<I, std::void_t<typename I::iterator_category>> {
    using type = I::iterator_category;
};
template <typename T> struct iter_tag<T *> {
    using type = mystd::random_access_iterator_tag;
};

template <typename I, typename Tag>
concept matches_iterator_tag =
    requires { typename iter_tag<I>::type; } && std::derived_from<typename iter_tag<I>::type, Tag>;

// clang-format off

/**
 * weakly_incrementable - object that can be incremented
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
 * incrementable - weakly_incrementable, but post-increment returns a copy
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
 * input_or_output_iterator - weakly_incrementable with dereferencing: an iterator
 * [semantics]
 * - equality preserving, *i == *i
 */
template <typename I>
concept input_or_output_iterator =
    weakly_incrementable<I> &&
    requires(I i) { *i; } &&
    can_reference<decltype(*std::declval<I>())>;

/**
 * output_iterator - input_or_output_iterator, but dereference must write-through
 * [semantics]
 * - sensible post-increment, *i++ = t is equivalent to *i = t; ++i;
 */
template <typename I, typename T>
concept output_iterator =
    input_or_output_iterator<I> &&
    std::indirectly_writable<I, T> &&
    requires (I i, T&& t) {
        *i++ = std::forward<T>(t);
    };

/**
 * input_iterator - input_or_output_iterator, but deference must read-through
 * [semantics]
 *  - none
 */
template <typename I>
concept input_iterator =
    input_or_output_iterator<I> &&
    std::indirectly_readable<I> &&
    matches_iterator_tag<I, input_iterator_tag>;

/**
 * forward_iterator - input_iterator (and optionally output_iterator), with semantics.
 * [semantics]
 *  - for i and j, comparison is defined only over the same sequence, or if value-initialized
 *  - multipass guarantee (i.e. copies are independent, as enabled by incrementable<T>'s copy semantics)
 *      - if i == j, then ++i == ++j
 *      - *i == ((void)[](auto x){ ++x; }(i), *i)
 */
template <typename I>
concept forward_iterator =
    input_iterator<I> &&
    incrementable<I> &&
    matches_iterator_tag<I, forward_iterator_tag>;

// clang-format on

} // namespace mystd
