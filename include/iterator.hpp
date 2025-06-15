#include <concepts>
#include <iterator>
#include <type_traits>

namespace mystd {

struct output_iterator_tag {};
struct input_iterator_tag {};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};

template <typename I> struct iter_concept {
    using type = typename I::iterator_category;
};
template <typename T> struct iter_concept<T *> {
    using type = random_access_iterator_tag;
};
template <typename I> using iter_concept_t = typename iter_concept<I>::type;

// clang-format off
template <typename I>
concept weakly_incrementable =
    std::movable<I> &&
    requires(I i) {
        { ++i } -> std::same_as<I &>;
        i++;
    };

template <typename I>
concept input_or_output_iterator =
    weakly_incrementable<I> &&
    requires(I i) { *i; } &&
    std::is_reference_v<decltype(*std::declval<I>())>;

template <typename I>
concept input_iterator =
    input_or_output_iterator<I> &&
    std::indirectly_readable<I> &&
    requires(I i) { typename iter_concept_t<I>; } &&
    std::derived_from<iter_concept_t<I>, input_iterator_tag>;
// clang-format on

} // namespace mystd
