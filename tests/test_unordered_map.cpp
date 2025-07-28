#include "type_traits.hpp"
#include "unordered_map.hpp"
#include "unordered_multimap.hpp"

#include <gtest/gtest.h>

// NOTE: These are smoke tests for the wrapper around detail::hashtable - see
// tests/hashtable/test_table.cpp.

using unordered_map = mystd::unordered_map<const char *, int>;

TEST(UnorderedMap, Aliases) {
    EXPECT_TRUE((mystd::is_same_v<unordered_map::key_type, const char *>));
    EXPECT_TRUE((mystd::is_same_v<unordered_map::mapped_type, int>));
    EXPECT_TRUE((mystd::is_same_v<unordered_map::value_type, std::pair<const char *, int>>));
}

TEST(UnorderedMap, Emplace) {
    unordered_map map;

    auto [it, inserted] = map.emplace("a", 1);
    EXPECT_EQ(it->first, "a");
    EXPECT_EQ(it->second, 1);
    EXPECT_TRUE(inserted);
    EXPECT_EQ(map.size(), 1);
}

TEST(UnorderedMap, Insert) {
    unordered_map map;
    std::pair<const char *, int> kv{"a", 1};

    map.insert(std::move(kv));
    auto [it, success] = map.insert({"a", 1});
    EXPECT_FALSE(success);

    map.insert({{"a", 1}, {"b", 1}});
    EXPECT_EQ(map.size(), 2);
}

TEST(UnorderedMap, Erase) {
    unordered_map map;
    map.insert({{"a", 1}, {"b", 1}, {"c", 1}, {"d", 1}});

    auto start_it = mystd::next(map.begin(), 1);
    auto end_it = mystd::next(map.begin(), 3);

    auto it = map.erase(start_it, end_it);
    EXPECT_EQ(it, end_it);
    EXPECT_EQ(map.size(), 2);
}

TEST(UnorderedMap, Merge) {
    unordered_map map;
    mystd::unordered_multimap<const char *, int> other;

    map.insert({"a", 1});
    other.insert({{"b", 2}, {"b", 3}});

    map.merge(other);
    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(other.size(), 0);

    size_t sum{};
    for (auto &[k, v] : map) {
        sum += v;
    }
    EXPECT_EQ(sum, 1 + 2);
}

TEST(UnorderedMap, Find) {
    unordered_map map;
    map.emplace("a", 1);

    auto it = map.find("a");
    EXPECT_EQ(it->first, "a");
    EXPECT_EQ(it->second, 1);

    EXPECT_EQ(map.find("NA"), map.end());
}

TEST(UnorderedMap, Contains) {
    unordered_map map;
    map.emplace("a", 1);

    EXPECT_TRUE(map.contains("a"));
    EXPECT_FALSE(map.contains("b"));
}

TEST(UnorderedMap, Subscript) {
    unordered_map map;

    map["a"] = 1;
    EXPECT_EQ(map["a"], 1);
    EXPECT_EQ(map.size(), 1);

    map[std::move("a")] = 2;

    auto it = map.find("a");
    EXPECT_EQ(it->first, "a");
    EXPECT_EQ(it->second, 2);
}

TEST(UnorderedMap, At) {
    unordered_map map;

    map["a"] = 1;
    EXPECT_EQ(map.at("a"), 1);

    map.at("a") = 2;
    EXPECT_EQ(map.at("a"), 2);

    EXPECT_THROW(map.at("b"), std::out_of_range);
    EXPECT_THROW(map.at("b") = 100, std::out_of_range);
}

TEST(UnorderedMap, EqualRange) {
    unordered_map map;
    map.emplace("a", 1);

    auto [first, last] = map.equal_range("a");
    EXPECT_EQ(first, map.find("a"));
    EXPECT_EQ(last, map.end());
}

TEST(UnorderedMap, Count) {
    unordered_map map;

    map.emplace("a", 1);
    EXPECT_EQ(map.count("a"), 1);
}
