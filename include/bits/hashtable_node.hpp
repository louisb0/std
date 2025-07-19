#pragma once

#include "bits/iterator_base_types.hpp"

#include <cstddef>
#include <memory>
#include <type_traits>

namespace mystd::detail {

template <typename T> struct node {
    node *next{};
    std::size_t hash{};
    T data{};
};

template <typename T, bool IsConst = false> class node_iterator {
    template <typename U, bool OtherConst> friend class node_iterator;

    struct node<T> *_node{};

public:
    using iterator_category = mystd::forward_iterator_tag;
    using value_type = T;
    using pointer = std::conditional_t<IsConst, const T *, T *>;
    using reference = std::conditional_t<IsConst, const T &, T &>;
    using difference_type = std::ptrdiff_t;

    node_iterator() = default;
    explicit node_iterator(struct node<T> *node) : _node(node) {}
    template <bool OtherConst>
    explicit node_iterator(const node_iterator<T, OtherConst> &other)
        requires(IsConst || !OtherConst)
        : _node(other._node) {}

    node_iterator &operator++() noexcept {
        _node = _node->next;
        return *this;
    }

    node_iterator operator++(int) noexcept {
        node_iterator tmp = *this;
        _node = _node->next;
        return tmp;
    }

    reference operator*() const noexcept { return _node->data; }
    pointer operator->() const noexcept { return std::addressof(_node->data); }

    struct node<T> *node() { return _node; }

    template <bool OtherConst>
    friend bool operator==(const node_iterator &lhs, const node_iterator<T, OtherConst> &rhs) {
        return lhs._node == rhs._node;
    }
};

template <typename T, bool IsConst = false> class local_node_iterator {
    template <typename U, bool OtherConst> friend class local_node_iterator;

    struct node<T> *_node{};
    std::size_t _bucket{};
    std::size_t _bucket_count{};

public:
    using iterator_category = mystd::forward_iterator_tag;
    using value_type = T;
    using pointer = std::conditional_t<IsConst, const T *, T *>;
    using reference = std::conditional_t<IsConst, const T &, T &>;
    using difference_type = std::ptrdiff_t;

    local_node_iterator() = default;
    explicit local_node_iterator(struct node<T> *node, std::size_t bucket, std::size_t bucket_count)
        : _node(node), _bucket(bucket), _bucket_count(bucket_count) {}

    template <bool OtherConst>
    explicit local_node_iterator(const local_node_iterator<T, OtherConst> &other)
        requires(IsConst || !OtherConst)
        : _node(other._node), _bucket(other._bucket), _bucket_count(other._bucket_count) {}

    local_node_iterator &operator++() noexcept {
        _node = _node->next;
        if (_node && _node->hash % _bucket_count != _bucket) {
            _node = nullptr;
        }

        return *this;
    }

    local_node_iterator operator++(int) noexcept {
        local_node_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    reference operator*() const noexcept { return _node->data; }
    pointer operator->() const noexcept { return std::addressof(_node->data); }

    struct node<T> *node() { return _node; }

    template <bool OtherConst>
    friend bool operator==(const local_node_iterator &lhs,
                           const local_node_iterator<T, OtherConst> &rhs) {
        return lhs._node == rhs._node;
    }
};

} // namespace mystd::detail
