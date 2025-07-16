#pragma once

#include "utility.hpp"

#include <type_traits>

namespace mystd {

template <typename T> class allocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::true_type;

    allocator() noexcept = default;
    allocator(const allocator &other) noexcept = default;
    template <typename U> allocator(const allocator<U> &other) noexcept {};

    ~allocator() = default;

    T *allocate(size_type n) { return static_cast<T *>(::operator new(sizeof(T) * n)); }
    void deallocate(T *p, size_type n) { ::operator delete(p); }
};

template <typename T1, typename T2>
bool operator==(const allocator<T1> &lhs, const allocator<T2> &rhs) {
    return true;
}

template <typename A, typename = void> struct get_is_always_equal {
    using type = std::false_type;
};
template <typename A> struct get_is_always_equal<A, std::void_t<typename A::is_always_equal>> {
    using type = typename A::is_always_equal;
};

template <typename A, typename = void> struct get_propagate_on_container_copy_assignment {
    using type = std::false_type;
};
template <typename A>
struct get_propagate_on_container_copy_assignment<
    A, std::void_t<typename A::propagate_on_container_copy_assignment>> {
    using type = typename A::propagate_on_container_copy_assignment;
};

template <typename A, typename = void> struct get_propagate_on_container_move_assignment {
    using type = std::false_type;
};
template <typename A>
struct get_propagate_on_container_move_assignment<
    A, std::void_t<typename A::propagate_on_container_move_assignment>> {
    using type = typename A::propagate_on_container_move_assignment;
};

template <typename A, typename = void> struct get_propagate_on_container_swap {
    using type = std::false_type;
};
template <typename A>
struct get_propagate_on_container_swap<A, std::void_t<typename A::propagate_on_container_swap>> {
    using type = typename A::propagate_on_container_swap;
};

template <typename A> struct allocator_traits {
    using allocator_type = A;
    using value_type = typename A::value_type;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using size_type = typename A::size_type;
    using is_always_equal = typename get_is_always_equal<A>::type;

    using propagate_on_container_copy_assignment =
        typename get_propagate_on_container_copy_assignment<A>::type;
    using propagate_on_container_move_assignment =
        typename get_propagate_on_container_move_assignment<A>::type;
    using propagate_on_container_swap = typename get_propagate_on_container_swap<A>::type;

    static pointer allocate(A &a, size_type n) { return a.allocate(n); }
    static void deallocate(A &a, pointer p, size_type n) { a.deallocate(p, n); }

    template <typename T, typename... Args> static void construct(A &a, T *p, Args &&...args) {
        ::new (static_cast<void *>(p)) T(mystd::forward<Args>(args)...);
    }
    template <typename T> static void destroy(A &a, T *p) { p->~T(); }

    static A select_on_container_copy_construction(const A &a) { return a; }
};

} // namespace mystd
