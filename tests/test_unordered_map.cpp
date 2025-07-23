#include "type_traits.hpp"
#include "unordered_map.hpp"

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

TEST(UnorderedMap, Find) {
    unordered_map map;
    map.emplace("a", 1);

    auto it = map.find("a");
    EXPECT_EQ(it->first, "a");
    EXPECT_EQ(it->second, 1);

    EXPECT_EQ(map.find("NA"), map.end());
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
