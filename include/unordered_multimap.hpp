#pragma once

#include "bits/hashtable.hpp"

#include "utility.hpp"

#include <functional>

namespace mystd {

template <typename K, typename V, typename Hash = std::hash<K>> class unordered_multimap {
    using _hashtable = detail::hashtable<std::pair<K, V>, detail::key_extractor_first, Hash, false>;
    _hashtable _table;

public:
    using key_type = typename _hashtable::key_type;
    using mapped_type = V;
    using value_type = typename _hashtable::value_type;
    using size_type = typename _hashtable::size_type;
    using iterator = typename _hashtable::iterator;
    using const_iterator = typename _hashtable::const_iterator;
    using local_iterator = typename _hashtable::local_iterator;
    using const_local_iterator = typename _hashtable::const_local_iterator;

    unordered_multimap() = default;
    unordered_multimap(size_type count) : _table(count) {}

    // Iterators.
    iterator begin() noexcept { return _table.begin(); }
    const_iterator begin() const noexcept { return _table.begin(); }
    const_iterator cbegin() const noexcept { return _table.cbegin(); }

    iterator end() noexcept { return _table.end(); }
    const_iterator end() const noexcept { return _table.end(); }
    const_iterator cend() const noexcept { return _table.cend(); }

    // Capacity.
    bool empty() const noexcept { return _table.empty(); }
    size_type size() const noexcept { return _table.size(); }
    size_type max_size() const noexcept { return _table.max_size(); }

    // Modifiers.
    template <typename... Args> iterator emplace(Args &&...args) {
        return _table.emplace(mystd::forward<Args>(args)...);
    }

    iterator insert(const value_type &value) { return _table.insert(value); }
    iterator insert(value_type &&value) { return _table.insert(std::move(value)); }
    template <mystd::input_iterator I> void insert(I first, I last) { _table.insert(first, last); }
    void insert(std::initializer_list<value_type> il) { _table.insert(il); }

    iterator erase(const_iterator pos) { return _table.erase(pos); }
    iterator erase(iterator pos) { return _table.erase(pos); }
    iterator erase(const_iterator first, const_iterator last) { return _table.erase(first, last); }
    size_type erase(const key_type &key) { return _table.erase(key); }

    void clear() noexcept { return _table.clear(); }

    // Lookup.
    iterator find(const key_type &key) noexcept { return _table.find(key); }
    const_iterator find(const key_type &key) const noexcept { return _table.find(key); }

    bool contains(const key_type &key) const noexcept { return _table.contains(key); }

    size_type count(const key_type &key) const noexcept { return _table.count(key); }

    std::pair<iterator, iterator> equal_range(const key_type &key) noexcept {
        return _table.equal_range(key);
    }
    std::pair<const_iterator, const_iterator> equal_range(const key_type &key) const noexcept {
        return _table.equal_range(key);
    }

    // Buckets.
    local_iterator begin(size_type bucket) noexcept { return _table.begin(bucket); }
    const_local_iterator begin(size_type bucket) const noexcept { return _table.begin(bucket); }
    const_local_iterator cbegin(size_type bucket) const noexcept { return _table.cbegin(bucket); }

    local_iterator end(size_type bucket) noexcept { return _table.end(bucket); }
    const_local_iterator end(size_type bucket) const noexcept { return _table.end(bucket); }
    const_local_iterator cend(size_type bucket) const noexcept { return _table.cend(bucket); }

    size_type bucket_count() const noexcept { return _table.bucket_count(); }
    size_type max_bucket_count() const noexcept { return _table.max_bucket_count(); }
    size_type bucket(const key_type &key) const noexcept { return _table.bucket(key); }
    size_type bucket_size(size_type bucket) const noexcept { return _table.bucket_size(bucket); }

    // Hashing.
    float load_factor() const noexcept { return _table.load_factor(); }
    float max_load_factor() const noexcept { return _table.max_load_factor(); }
    void max_load_factor(float ml) noexcept { _table.max_load_factor(ml); }
    void rehash(size_type count) { _table.rehash(count); }
    void reserve(size_type count) { _table.reserve(count); }
};

} // namespace mystd
