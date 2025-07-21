#include "type_traits.hpp"
#include "unordered_map.hpp"

#include <gtest/gtest.h>

TEST(UnorderedMap, Aliases) {
    using map = mystd::unordered_map<const char *, int>;

    EXPECT_TRUE((mystd::is_same_v<map::key_type, const char *>));
    EXPECT_TRUE((mystd::is_same_v<map::mapped_type, int>));
    EXPECT_TRUE((mystd::is_same_v<map::value_type, std::pair<const char *, int>>));
}

TEST(UnorderedMap, Emplace) {
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

TEST(UnorderedMap, Find) {
    mystd::unordered_map<const char *, int> map;
    map.emplace("a", 1);

    auto it = map.find("a");
    EXPECT_EQ(it->first, "a");
    EXPECT_EQ(it->second, 1);

    EXPECT_EQ(map.find("NA"), map.end());
}
