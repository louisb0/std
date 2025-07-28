#include "type_traits.hpp"
#include "unordered_map.hpp"
#include "unordered_multimap.hpp"

#include <gtest/gtest.h>

// NOTE: These are smoke tests for the wrapper around detail::hashtable - see
// tests/hashtable/test_table.cpp.

using unordered_multimap = mystd::unordered_multimap<const char *, int>;

TEST(UnorderedMultiMap, Aliases) {
    EXPECT_TRUE((mystd::is_same_v<unordered_multimap::key_type, const char *>));
    EXPECT_TRUE((mystd::is_same_v<unordered_multimap::mapped_type, int>));
    EXPECT_TRUE((mystd::is_same_v<unordered_multimap::value_type, std::pair<const char *, int>>));
}

TEST(UnorderedMultiMap, Emplace) {
    unordered_multimap map;

    auto first_it = map.emplace("a", 1);
    EXPECT_EQ(first_it->first, "a");
    EXPECT_EQ(first_it->second, 1);
    EXPECT_EQ(map.size(), 1);

    auto second_it = map.emplace("a", 1);
    EXPECT_NE(first_it, second_it);
    EXPECT_EQ(map.size(), 2);
}

TEST(UnorderedMultiMap, Insert) {
    unordered_multimap map;
    std::pair<const char *, int> kv{"a", 1};

    map.insert(std::move(kv));
    map.insert({"a", 1});
    map.insert({{"a", 1}, {"b", 1}});
    EXPECT_EQ(map.size(), 4);
}

TEST(UnorderedMultiMap, Erase) {
    unordered_multimap map;
    map.insert({{"a", 1}, {"c", 1}, {"c", 1}, {"d", 1}});

    EXPECT_EQ(map.erase("c"), 2);
    EXPECT_EQ(map.size(), 2);
}

TEST(UnorderedMutliMap, Merge) {
    unordered_multimap map;
    mystd::unordered_map<const char *, int> other;

    map.insert({"a", 1});
    other.insert({"a", 2});

    map.merge(other);
    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(other.size(), 0);

    size_t sum{};
    for (auto &[k, v] : map) {
        sum += v;
    }
    EXPECT_EQ(sum, 1 + 2);
}

TEST(UnorderedMultiMap, Find) {
    unordered_multimap map;
    map.emplace("a", 1);

    auto it = map.find("a");
    EXPECT_EQ(it->first, "a");
    EXPECT_EQ(it->second, 1);

    EXPECT_EQ(map.find("NA"), map.end());
}

TEST(UnorderedMultiMap, Contains) {
    unordered_multimap map;
    map.emplace("a", 1);

    EXPECT_TRUE(map.contains("a"));
    EXPECT_FALSE(map.contains("b"));
}

TEST(UnorderedMultiMap, EqualRange) {
    unordered_multimap map;
    map.emplace("b", 1);
    map.emplace("b", 2);
    map.emplace("b", 3);

    auto [first, last] = map.equal_range("b");
    EXPECT_EQ(mystd::distance(first, last), 3);
}

TEST(UnorderedMultiMap, Count) {
    unordered_multimap map;
    map.emplace("a", 1);
    map.emplace("a", 1);

    EXPECT_EQ(map.count("a"), 2);
    EXPECT_EQ(map.count("b"), 0);
}
