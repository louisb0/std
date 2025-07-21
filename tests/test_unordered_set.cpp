#include "type_traits.hpp"
#include "unordered_set.hpp"

#include <gtest/gtest.h>

TEST(UnorderedSet, Aliases) {
    using set = mystd::unordered_set<int>;

    EXPECT_TRUE((mystd::is_same_v<set::key_type, int>));
    EXPECT_TRUE((mystd::is_same_v<set::value_type, int>));
}

TEST(UnorderedSet, Emplace) {
    mystd::unordered_set<int> set;

    auto [new_it, new_inserted] = set.emplace(1);
    EXPECT_EQ(*new_it, 1);
    EXPECT_TRUE(new_inserted);
    EXPECT_EQ(set.size(), 1);

    auto [existing_it, duplicate_inserted] = set.emplace(1);
    EXPECT_EQ(existing_it, new_it);
    EXPECT_FALSE(duplicate_inserted);
    EXPECT_EQ(set.size(), 1);
}

TEST(UnorderedSet, Find) {
    mystd::unordered_set<int> set;
    set.emplace(1);

    auto it = set.find(1);
    EXPECT_EQ(*it, 1);

    EXPECT_EQ(set.find(2), set.end());
}
