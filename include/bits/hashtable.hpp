#pragma once

#include "algorithm.hpp"
#include "bits/hashtable_node.hpp"
#include "utility.hpp"

#include <limits>
#include <type_traits>
#include <utility>

namespace mystd::detail {

struct key_extractor_first {
    template <typename Pair> const auto &operator()(const Pair &p) const noexcept {
        return p.first;
    }
};

struct key_extractor_identity {
    template <typename T> const auto &operator()(const T &t) const noexcept { return t; }
};

template <typename V, typename KeyExtractor, typename Hash, bool Unique> class hashtable {
public:
    using value_type = V;
    using key_type =
        std::remove_cvref_t<decltype(std::declval<KeyExtractor>()(std::declval<value_type>()))>;
    using size_type = std::size_t;
    using iterator = detail::node_iterator<value_type, false>;
    using const_iterator = detail::node_iterator<value_type, true>;
    using local_iterator = detail::local_node_iterator<value_type, false>;
    using const_local_iterator = detail::local_node_iterator<value_type, true>;

private:
    using _return_type = std::conditional_t<Unique, std::pair<iterator, bool>, iterator>;
    using _node_type = detail::node<V>;

    size_type _element_count{};
    size_type _bucket_count{};
    _node_type _before_begin{};
    _node_type **_buckets{};

    Hash _hash;
    KeyExtractor _extract_key{};

public:
    hashtable() : hashtable(16) {}
    hashtable(size_type count) : _bucket_count(count), _buckets(new _node_type *[count] {}) {}

    ~hashtable() {
        clear();
        delete[] _buckets;
    }

    // Iterators.
    iterator begin() noexcept { return iterator(_before_begin.next); }
    const_iterator begin() const noexcept { return const_iterator(_before_begin.next); }
    const_iterator cbegin() const noexcept { return begin(); }

    iterator end() noexcept { return iterator(nullptr); }
    const_iterator end() const noexcept { return iterator(nullptr); }
    const_iterator cend() const noexcept { return end(); }

    // Capacity.
    bool empty() const noexcept { return _element_count == 0; }
    size_type size() const noexcept { return _element_count; }
    size_type max_size() const noexcept { return std::numeric_limits<size_type>::max(); }

    // Modifiers.
    template <typename... Args> _return_type emplace(Args &&...args) {
        value_type data(mystd::forward<Args>(args)...);
        const key_type &key = _extract_key(data);

        if constexpr (Unique) {
            if (auto existing = find(key); existing != end()) {
                return {existing, false};
            }
        }

        auto inserted = _insert_unconditional(new _node_type{
            .hash = _hash(key),
            .data = std::move(data),
        });

        if constexpr (Unique) {
            return {inserted, true};
        } else {
            return inserted;
        }
    }

    void clear() noexcept {
        _node_type *cur = _before_begin.next;
        while (cur) {
            _node_type *next = cur->next;
            delete cur;
            cur = next;
        }

        mystd::fill(_buckets, _buckets + _bucket_count, nullptr);
        _before_begin.next = nullptr;
    }

    // Lookup.
    iterator find(const key_type &key) noexcept {
        size_type bucket = _hash(key) % _bucket_count;

        for (auto it = begin(bucket); it != end(bucket); ++it) {
            if (_extract_key(*it) == key) {
                return iterator(it.node());
            }
        }

        return end();
    }

    const_iterator find(const key_type &key) const noexcept {
        return const_iterator(const_cast<hashtable *>(this)->find(key));
    }

    // Buckets.
    local_iterator begin(size_type bucket) noexcept {
        return local_iterator(_bucket_begin(bucket), bucket, _bucket_count);
    }
    const_local_iterator begin(size_type bucket) const noexcept {
        return const_local_iterator(_bucket_begin(bucket), bucket, _bucket_count);
    }
    const_local_iterator cbegin(size_type bucket) const noexcept { return begin(bucket); }

    local_iterator end(size_type bucket) noexcept {
        return local_iterator(nullptr, bucket, _bucket_count);
    }
    const_local_iterator end(size_type bucket) const noexcept {
        return const_local_iterator(nullptr, bucket, _bucket_count);
    }
    const_local_iterator cend(size_type bucket) const noexcept { return end(bucket); }

private:
    iterator _insert_unconditional(_node_type *node) noexcept {
        size_type bucket = node->hash % _bucket_count;

        if (_buckets[bucket]) {
            node->next = _buckets[bucket]->next;
            _buckets[bucket]->next = node;
        } else {
            node->next = _before_begin.next;
            _before_begin.next = node;

            if (node->next) {
                _buckets[node->next->hash % _bucket_count] = node;
            }
            _buckets[bucket] = &_before_begin;
        }

        ++_element_count;
        return iterator(node);
    }

    _node_type *_bucket_begin(size_type bucket) const noexcept {
        return _buckets[bucket] ? _buckets[bucket]->next : nullptr;
    }
};

} // namespace mystd::detail
