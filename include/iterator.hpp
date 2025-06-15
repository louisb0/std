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

template <typename I> struct iter_tag {
    using type = typename I::iterator_category;
};
template <typename T> struct iter_tag<T *> {
    using type = random_access_iterator_tag;
};
template <typename I> using iter_tag_t = typename iter_tag<I>::type;

// clang-format off
template <typename I>
concept weakly_incrementable =
    std::movable<I> &&
    requires(I i) {
        { ++i } -> std::same_as<I &>;
        i++;
    };
 /* semantics {
        [for a state i, either both ++i and i++ are valid or both are invalid]
        domain(++i) == domain(i++)

        [both forms must move to the same next iterator state]
        if incrementable(i): ++i and i++ both advance i

        [++i must return a reference to the same object i]
        if incrementable(i): addressof(++i) == addressof(i)
    } */

template <typename I>
concept input_or_output_iterator =
    weakly_incrementable<I> &&
    requires(I i) { *i; } &&
    can_reference<decltype(*std::declval<I>())>;
 /* semantics {
        [equality preserving]
        *i == *i
    } */

template <typename I>
concept input_iterator =
    input_or_output_iterator<I> &&
    std::indirectly_readable<I> &&
    requires(I i) { typename iter_tag_t<I>; } &&
    std::derived_from<iter_tag_t<I>, input_iterator_tag>;

template <typename I, typename T>
concept output_iterator =
    input_or_output_iterator<I> &&
    std::indirectly_writable<I, T> &&
    requires (I i, T&& t) {
        *i++ = std::forward<T>(t);
    };
 /* semantics {
        [post-increment cannot pre-emptively changing the write position]
        *i++ = t  <=>  *i = t; ++i;
    } */

template <typename I>
concept forward_iterator =
    input_iterator<I> &&
    requires(I i) { typename iter_tag_t<I>; } &&
    std::derived_from<iter_tag_t<I>, forward_iterator_tag>;
 /* semantics {
        [comparison between i and j is defined only over the same sequence or both value-initialized]
        defined(i == j) <=> (sequence(i) == sequence(j)) || (i{} && j{})

        [pointers / references from the iterator remain valid while sequence exists]
        lifetime(pointer_from(i)) == lifetime(reference_from(i)) == lifetime(sequence())

        [multipass guarantee - copies don't affect originals]
            if (i == j): ++i == ++j
            increment_copy(i) doesn't change *i
    } */

// clang-format on

} // namespace mystd
