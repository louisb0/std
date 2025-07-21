#include "type_traits.hpp"
#include "unordered_map.hpp"
#include "unordered_multimap.hpp"

#include <gtest/gtest.h>

TEST(UnorderedMap, Aliases) {
    using map = mystd::unordered_map<const char *, int>;

    EXPECT_TRUE((mystd::is_same_v<map::key_type, const char *>));
    EXPECT_TRUE((mystd::is_same_v<map::mapped_type, int>));
    EXPECT_TRUE((mystd::is_same_v<map::value_type, std::pair<const char *, int>>));
}

TEST(UnorderedMap, UniqueEmplace) {
    mystd::unordered_map<const char *, int> map;

    auto [new_it, new_inserted] = map.emplace("a", 1);
    EXPECT_EQ(new_it->first, "a");
    EXPECT_EQ(new_it->second, 1);
    EXPECT_TRUE(new_inserted);
    EXPECT_EQ(map.size(), 1);

    auto [existing_it, duplicate_inserted] = map.emplace("a", 2);
    EXPECT_EQ(existing_it, new_it);
    EXPECT_EQ(existing_it->second, 1);
    EXPECT_FALSE(duplicate_inserted);
    EXPECT_EQ(map.size(), 1);
}

TEST(UnorderedMap, MultiEmplace) {
    mystd::unordered_multimap<const char *, int> map;

    auto first_it = map.emplace("a", 1);
    EXPECT_EQ(first_it->first, "a");
    EXPECT_EQ(first_it->second, 1);
    EXPECT_EQ(map.size(), 1);

    auto second_it = map.emplace("a", 2);
    EXPECT_NE(first_it, second_it);
    EXPECT_EQ(second_it->first, "a");
    EXPECT_EQ(second_it->second, 2);
    EXPECT_EQ(map.size(), 2);
}

TEST(UnorderedMap, Find) {
    mystd::unordered_map<const char *, int> map;
    map.emplace("a", 1);

    auto it = map.find("a");
    EXPECT_EQ(it->first, "a");
    EXPECT_EQ(it->second, 1);

    EXPECT_EQ(map.find("NA"), map.end());
}

TEST(UnorderedMap, UniqueCount) {
    mystd::unordered_map<const char *, int> map;
    map.emplace("a", 1);

    EXPECT_EQ(map.count("a"), 1);
    EXPECT_EQ(map.count("b"), 0);
}

TEST(UnorderedMap, MultiCount) {
    mystd::unordered_multimap<const char *, int> map;
    map.emplace("a", 1);
    map.emplace("a", 1);

    EXPECT_EQ(map.count("a"), 2);
    EXPECT_EQ(map.count("b"), 0);
}
