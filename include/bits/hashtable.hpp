#pragma once

#include "algorithm.hpp"
#include "bits/hashtable_node.hpp"
#include "bits/iterator_concepts.hpp"
#include "bits/iterator_functions.hpp"
#include "utility.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
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
    static constexpr bool is_set = std::is_same_v<KeyExtractor, key_extractor_identity>;

public:
    using value_type = V;
    using key_type =
        std::remove_cvref_t<decltype(std::declval<KeyExtractor>()(std::declval<value_type>()))>;
    using size_type = std::size_t;
    using iterator = detail::node_iterator<value_type, is_set>;
    using const_iterator = detail::node_iterator<value_type, true>;
    using local_iterator = detail::local_node_iterator<value_type, is_set>;
    using const_local_iterator = detail::local_node_iterator<value_type, true>;

private:
    using _return_type = std::conditional_t<Unique, std::pair<iterator, bool>, iterator>;
    using _node_type = detail::node<V>;

    size_type _element_count{};
    size_type _bucket_count{};
    _node_type _before_begin{};
    _node_type **_buckets{};
    float _max_load_factor{0.75};

    Hash _hash{};
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
    const_iterator end() const noexcept { return const_iterator(nullptr); }
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
            .data = mystd::move(data),
        });

        if (load_factor() > max_load_factor()) {
            rehash(2 * bucket_count());
        }

        if constexpr (Unique) {
            return {inserted, true};
        } else {
            return inserted;
        }
    }

    _return_type insert(const value_type &value) { return emplace(value); }
    _return_type insert(value_type &&value) { return emplace(std::move(value)); }

    template <mystd::input_iterator I> void insert(I first, I last) {
        for (; first != last; ++first) {
            insert(*first);
        }
    }
    void insert(std::initializer_list<value_type> il) { insert(il.begin(), il.end()); }

    iterator erase(const_iterator pos) {
        _node_type *to_delete = pos.node();
        _node_type *prev = _get_previous(to_delete);

        size_type bucket = _bucket(to_delete);
        size_type next_bucket = to_delete->next ? _bucket(to_delete->next) : bucket;

        if (next_bucket != bucket) {
            _buckets[next_bucket] = prev;

            if (prev == _buckets[bucket]) {
                _buckets[bucket] = nullptr;
            }
        }

        prev->next = to_delete->next;
        delete to_delete;
        --_element_count;

        return iterator(prev->next);
    }

    iterator erase(iterator pos)
        requires(!is_set || !std::same_as<iterator, const_iterator>)
    {
        return erase(const_iterator(pos));
    }

    // NOTE: Inefficient hack.
    iterator erase(const_iterator first, const_iterator last) {
        while (first != last) {
            first = erase(first);
        }
        return iterator(last.node());
    }

    size_type erase(const key_type &key) {
        if constexpr (Unique) {
            auto it = find(key);
            if (it == end()) {
                return 0;
            }
            erase(it);
            return 1;
        } else {
            auto [first, last] = equal_range(key);
            size_type count = 0;
            while (first != last) {
                first = erase(first);
                ++count;
            }
            return count;
        }
    }

    void clear() noexcept {
        _node_type *cur = _before_begin.next;
        while (cur) {
            _node_type *next = cur->next;
            delete cur;
            cur = next;
        }

        mystd::fill(_buckets, _buckets + bucket_count(), nullptr);
        _before_begin.next = nullptr;
    }

    // Lookup.
    iterator find(const key_type &key) noexcept {
        size_type bucket = this->bucket(key);

        for (auto it = begin(bucket); it != end(bucket); ++it) {
            if (_extract_key(*it) == key) {
                return iterator(it.node());
            }
        }

        return end();
    }

    const_iterator find(const key_type &key) const noexcept {
        return const_cast<hashtable *>(this)->find(key);
    }

    bool contains(const key_type &key) const noexcept { return find(key) != end(); }

    size_type count(const key_type &key) const noexcept {
        if constexpr (Unique) {
            return find(key) != end() ? 1 : 0;
        } else {
            auto [first, last] = equal_range(key);
            return mystd::distance(first, last);
        }
    }

    std::pair<iterator, iterator> equal_range(const key_type &key) noexcept {
        if constexpr (Unique) {
            auto first = find(key);
            auto second = (first == end()) ? end() : mystd::next(first);

            return {first, second};
        } else {
            size_type bucket = this->bucket(key);
            auto pred = [&](const auto &elem) { return _extract_key(elem) == key; };

            local_iterator bucket_first = mystd::find_if(begin(bucket), end(bucket), pred);

            iterator first = iterator(bucket_first.node());
            iterator last = mystd::find_if_not(first, end(), pred);

            return {first, last};
        }
    }

    std::pair<const_iterator, const_iterator> equal_range(const key_type &key) const noexcept {
        auto [first, last] = const_cast<hashtable *>(this)->equal_range(key);
        return {first, last};
    }

    // Buckets.
    local_iterator begin(size_type bucket) noexcept {
        _node_type *bucket_start = _buckets[bucket] ? _buckets[bucket]->next : nullptr;
        return local_iterator(bucket_start, bucket, bucket_count());
    }
    const_local_iterator begin(size_type bucket) const noexcept {
        _node_type *bucket_start = _buckets[bucket] ? _buckets[bucket]->next : nullptr;
        return const_local_iterator(bucket_start, bucket, bucket_count());
    }
    const_local_iterator cbegin(size_type bucket) const noexcept { return begin(bucket); }

    local_iterator end(size_type bucket) noexcept {
        return local_iterator(nullptr, bucket, bucket_count());
    }
    const_local_iterator end(size_type bucket) const noexcept {
        return const_local_iterator(nullptr, bucket, bucket_count());
    }
    const_local_iterator cend(size_type bucket) const noexcept { return end(bucket); }

    size_type bucket_count() const noexcept { return _bucket_count; }
    size_type max_bucket_count() const noexcept { return std::numeric_limits<size_type>::max(); }
    size_type bucket(const key_type &key) const noexcept { return _hash(key) % bucket_count(); }
    size_type bucket_size(size_type bucket) const noexcept {
        return mystd::distance(begin(bucket), end(bucket));
    }

    // Hashing.
    float load_factor() const noexcept { return size() / bucket_count(); }
    float max_load_factor() const noexcept { return _max_load_factor; }
    void max_load_factor(float ml) noexcept { _max_load_factor = ml; }

    void rehash(size_type count) {
        size_type new_bucket_count =
            std::max(count, static_cast<size_type>(std::ceil(size() / max_load_factor())));
        _node_type **new_buckets = new _node_type *[new_bucket_count] {};

        _node_type *cur = _before_begin.next;
        _before_begin.next = nullptr;

        // NOTE: It is invariant per _insert_unconditional() that elements which compare equal are
        // stored 'contiguously' in the linked list. So, when rehashed, they too will be stored
        // contiguous without any special casing.
        while (cur) {
            _node_type *next = cur->next;

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

            cur = next;
        }

        delete[] _buckets;
        _buckets = new_buckets;
        _bucket_count = new_bucket_count;
    }

    void reserve(size_type count) {
        rehash(static_cast<size_type>(std::ceil(count / max_load_factor())));
    }

private:
    iterator _insert_unconditional(_node_type *node) noexcept {
        size_type bucket = _bucket(node);

        if (_buckets[bucket]) {
            _node_type *insert_after;
            if constexpr (Unique) {
                insert_after = _buckets[bucket];
            } else {
                auto [first, last] = equal_range(_extract_key(node->data));
                insert_after = (first != last) ? first.node() : _buckets[bucket];
            }

            node->next = insert_after->next;
            insert_after->next = node;
        } else {
            node->next = _before_begin.next;
            _before_begin.next = node;

            if (node->next) {
                _buckets[_bucket(node->next)] = node;
            }
            _buckets[bucket] = &_before_begin;
        }

        ++_element_count;
        return iterator(node);
    }

    _node_type *_get_previous(_node_type *node) {
        _node_type *prev = _buckets[_bucket(node)];
        while (prev && prev->next != node) {
            prev = prev->next;
        }

        return prev;
    }

    size_type _bucket(_node_type *node) { return node->hash % bucket_count(); }
};

} // namespace mystd::detail
