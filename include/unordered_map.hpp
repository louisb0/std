#pragma once

#include "bits/hashtable_node.hpp"
#include "iterator.hpp"
#include "utility.hpp"

#include <functional>
#include <utility>

namespace mystd {

template <typename K, typename T, typename Hash = std::hash<K>> class unordered_map {
public:
    using key_type = K;
    using mapped_type = T;
    using value_type = std::pair<const K, T>;
    using size_type = std::size_t;
    using iterator = mystd::detail::node_iterator<value_type>;
    using const_iterator = mystd::detail::node_iterator<value_type, true>;
    using local_iterator = mystd::detail::local_node_iterator<value_type>;
    using const_local_iterator = mystd::detail::local_node_iterator<value_type, true>;
    using node_type = mystd::detail::node<std::pair<const K, T>>;

private:
    size_type _bucket_count{};
    size_type _element_count{};
    node_type _before_begin{};
    node_type **_buckets{};

    Hash _hasher{};

public:
    unordered_map() : unordered_map(size_type(16)) {}
    unordered_map(size_type bucket_count)
        : _bucket_count(bucket_count), _buckets(new node_type *[bucket_count] {}) {}

    // Capacity.
    bool empty() const noexcept { return _element_count == 0; }
    size_type size() const noexcept { return _element_count; }
    size_type max_size() const noexcept { return std::numeric_limits<size_type>::max(); }

    // Access.
    iterator find(const key_type &key) {
        size_type hash = _hasher(key);
        size_type bucket = hash % bucket_count();
        if (!_buckets[bucket]) {
            return end();
        }

        node_type *cur = _buckets[bucket]->next;
        while (cur && (cur->hash % bucket_count() == bucket)) {
            if (cur->data.first == key) {
                return iterator(cur);
            }

            cur = cur->next;
        }

        return end();
    }

    // Modifiers.
    template <typename... Args> std::pair<iterator, bool> emplace(Args &&...args) {
        value_type data(mystd::forward<Args>(args)...);
        if (find(data.first) != end()) {
            return std::make_pair(end(), false);
        }

        size_type hash = _hasher(data.first);
        size_type bucket = hash % bucket_count();

        node_type *node = new node_type{.hash = hash, .data = std::move(data)};

        if (_buckets[bucket]) {
            node->next = _buckets[bucket];
            _buckets[bucket]->next = node;
        } else {
            node->next = _before_begin.next;
            _before_begin.next = node;

            if (node->next) {
                _buckets[node->next->hash % bucket_count()] = node;
            }

            _buckets[bucket] = &_before_begin;
        }

        _element_count++;
        return {iterator(node), true};
    }

    // Iterators.
    iterator begin() noexcept { return iterator(_before_begin.next); }
    const_iterator begin() const noexcept { return cbegin(); }
    iterator cbegin() const noexcept { return const_iterator(_before_begin.next); }

    iterator end() noexcept { return iterator(nullptr); }
    const_iterator end() const noexcept { return cend(); }
    iterator cend() const noexcept { return const_iterator(nullptr); }

    // Buckets.
    size_type bucket_count() const noexcept { return _bucket_count; }
    size_type max_bucket_count() const noexcept { return std::numeric_limits<size_type>::max(); }
    size_type bucket(const key_type &key) const noexcept { return _hasher(key) % _bucket_count; }
    size_type bucket_size(size_type bucket) const noexcept {
        return mystd::distance(begin(bucket), end(bucket));
    }

    local_iterator begin(size_type bucket) noexcept {
        node_type *node = _buckets[bucket] ? _buckets[bucket]->next : nullptr;
        return local_iterator(node, bucket, _bucket_count);
    }
    const_local_iterator begin(size_type bucket) const noexcept { return cbegin(bucket); }
    const_local_iterator cbegin(size_type bucket) const noexcept {
        node_type *node = _buckets[bucket] ? _buckets[bucket]->next : nullptr;
        return const_local_iterator(node, bucket, _bucket_count);
    }

    local_iterator end(size_type bucket) noexcept {
        return local_iterator(nullptr, bucket, _bucket_count);
    }
    local_iterator end(size_type bucket) const noexcept { return cend(bucket); }
    const_local_iterator cend(size_type bucket) const noexcept {
        return const_local_iterator(nullptr, bucket, _bucket_count);
    }
};

} // namespace mystd
