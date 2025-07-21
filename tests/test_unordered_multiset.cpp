#include "type_traits.hpp"
#include "unordered_multiset.hpp"

#include <gtest/gtest.h>

TEST(UnorderedMultiSet, Aliases) {
    using set = mystd::unordered_multiset<int>;

    EXPECT_TRUE((mystd::is_same_v<set::key_type, int>));
    EXPECT_TRUE((mystd::is_same_v<set::value_type, int>));
}

TEST(UnorderedMultiSet, Emplace) {
    mystd::unordered_multiset<int> set;

    auto first_it = set.emplace(1);
    EXPECT_EQ(*first_it, 1);
    EXPECT_EQ(set.size(), 1);

    auto second_it = set.emplace(1);
    EXPECT_EQ(*second_it, 1);
    EXPECT_EQ(set.size(), 2);
    EXPECT_NE(first_it, second_it);
}

TEST(UnorderedMultiSet, Find) {
    mystd::unordered_multiset<int> set;
    set.emplace(1);

    auto it = set.find(1);
    EXPECT_EQ(*it, 1);

    EXPECT_EQ(set.find(2), set.end());
}

TEST(UnorderedMultiSet, Count) {
    mystd::unordered_multiset<int> set;
    set.emplace(1);
    set.emplace(1);

    EXPECT_EQ(set.count(1), 2);
    EXPECT_EQ(set.count(2), 0);
}
