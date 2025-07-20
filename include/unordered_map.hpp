#pragma once

#include "bits/hashtable_node.hpp"
#include "iterator.hpp"
#include "utility.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <stdexcept>
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

    float _max_load_factor = 0.75;
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
    iterator find(const key_type &key) noexcept {
        size_type bkt = bucket(key);
        for (auto it = begin(bkt); it != end(bkt); ++it) {
            if (it->first == key) {
                return iterator(it.node());
            }
        }

        return end();
    }

    const_iterator find(const key_type &key) const noexcept {
        return const_iterator(const_cast<unordered_map *>(this)->find(key).node());
    }

    mapped_type &at(const key_type &key) {
        auto it = find(key);
        if (it == end()) {
            throw std::out_of_range(
                "mystd::unordered_map::at() was called with a non-existent key.");
        }

        return it->second;
    }

    const mapped_type &at(const key_type &key) const {
        return const_cast<unordered_map *>(this)->at(key);
    }

    mapped_type &operator[](const key_type &key) noexcept {
        auto it = find(key);
        if (it != end()) {
            return it->second;
        }

        auto [inserted, _] = emplace(key, mapped_type{});
        return inserted->second;
    }

    bool contains(const key_type &key) const noexcept { return find(key) != end(); }
    size_type count(const key_type &key) const noexcept { return contains(key) ? 1 : 0; }

    // Modifiers.
    template <typename... Args> std::pair<iterator, bool> emplace(Args &&...args) {
        value_type data(mystd::forward<Args>(args)...);
        auto it = find(data.first);
        if (it != end()) {
            return std::make_pair(it, false);
        }

        size_type hash = _hasher(data.first);
        size_type bucket = hash % bucket_count();

        node_type *node = new node_type{.hash = hash, .data = std::move(data)};

        if (_buckets[bucket]) {
            node->next = _buckets[bucket]->next;
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
        if (load_factor() > max_load_factor()) {
            rehash(2 * size());
        }

        return {iterator(node), true};
    }

    // Iterators.
    iterator begin() noexcept { return iterator(_before_begin.next); }
    const_iterator begin() const noexcept { return cbegin(); }
    const_iterator cbegin() const noexcept { return const_iterator(_before_begin.next); }

    iterator end() noexcept { return iterator(nullptr); }
    const_iterator end() const noexcept { return cend(); }
    const_iterator cend() const noexcept { return const_iterator(nullptr); }

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
    const_local_iterator end(size_type bucket) const noexcept { return cend(bucket); }
    const_local_iterator cend(size_type bucket) const noexcept {
        return const_local_iterator(nullptr, bucket, _bucket_count);
    }

    // Policy.
    float load_factor() const noexcept { return size() / bucket_count(); }
    float max_load_factor() const noexcept { return _max_load_factor; }
    void max_load_factor(float ml) noexcept { _max_load_factor = ml; }
    void rehash(size_type count) {
        size_type new_bucket_count =
            std::max(count, static_cast<size_type>(std::ceil(size() / _max_load_factor)));
        node_type **new_buckets = new node_type *[new_bucket_count] {};

        node_type *cur = _before_begin.next;
        _before_begin.next = nullptr;

        for (node_type *next; cur; cur = next) {
            next = cur->next;

            size_type bucket = cur->hash % new_bucket_count;

            if (new_buckets[bucket]) {
                cur->next = new_buckets[bucket]->next;
                new_buckets[bucket]->next = cur;
            } else {
                cur->next = _before_begin.next;
                _before_begin.next = cur;

                if (cur->next) {
                    new_buckets[cur->next->hash % new_bucket_count] = cur;
                }

                new_buckets[bucket] = &_before_begin;
            }
        }

        delete[] _buckets;
        _buckets = new_buckets;
        _bucket_count = new_bucket_count;
    }
};

} // namespace mystd
