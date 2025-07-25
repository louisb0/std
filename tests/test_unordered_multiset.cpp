#include "type_traits.hpp"
#include "unordered_multiset.hpp"

#include <gtest/gtest.h>

// NOTE: These are smoke tests for the wrapper around detail::hashtable - see
// tests/hashtable/test_table.cpp.

using unordered_multiset = mystd::unordered_multiset<int>;

TEST(UnorderedMultiSet, Aliases) {
    EXPECT_TRUE((mystd::is_same_v<unordered_multiset::key_type, int>));
    EXPECT_TRUE((mystd::is_same_v<unordered_multiset::value_type, int>));
}

TEST(UnorderedMultiSet, Emplace) {
    unordered_multiset set;

    auto first_it = set.emplace(1);
    EXPECT_EQ(*first_it, 1);
    EXPECT_EQ(set.size(), 1);

    auto second_it = set.emplace(1);
    EXPECT_EQ(*second_it, 1);
    EXPECT_EQ(set.size(), 2);
    EXPECT_NE(first_it, second_it);
}

TEST(UnorderedMultiSet, Insert) {
    unordered_multiset set;
    int val = 1;

    set.insert(std::move(val));
    set.insert(1);
    set.insert({1, 1});
    EXPECT_EQ(set.size(), 4);
}

TEST(UnorderedMultiSet, Erase) {
    unordered_multiset set;
    set.insert({1, 2, 2, 3});

    EXPECT_EQ(set.erase(2), 2);
    EXPECT_EQ(set.size(), 2);
}

TEST(UnorderedMultiSet, Find) {
    unordered_multiset set;
    set.emplace(1);

    EXPECT_EQ(*set.find(1), 1);
    EXPECT_EQ(set.find(2), set.end());
}

TEST(UnorderedMultiSet, Contains) {
    unordered_multiset set;
    set.emplace(1);

    EXPECT_TRUE(set.contains(1));
    EXPECT_FALSE(set.contains(2));
}

TEST(UnorderedMultiSet, EqualRange) {
    unordered_multiset set;
    set.emplace(1);
    set.emplace(1);
    set.emplace(1);

    auto [first, last] = set.equal_range(1);
    EXPECT_EQ(mystd::distance(first, last), 3);

    for (auto it = first; it != last; ++it) {
        EXPECT_EQ(*it, 1);
    }
}

TEST(UnorderedMultiSet, Count) {
    unordered_multiset set;
    set.emplace(1);
    set.emplace(1);

    EXPECT_EQ(set.count(1), 2);
    EXPECT_EQ(set.count(2), 0);
}
