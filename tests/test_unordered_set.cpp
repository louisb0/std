#include "type_traits.hpp"
#include "unordered_set.hpp"

#include <gtest/gtest.h>

// NOTE: These are smoke tests for the wrapper around detail::hashtable - see
// tests/hashtable/test_table.cpp.

using unordered_set = mystd::unordered_set<int>;

TEST(UnorderedSet, Aliases) {
    EXPECT_TRUE((mystd::is_same_v<unordered_set::key_type, int>));
    EXPECT_TRUE((mystd::is_same_v<unordered_set::value_type, int>));
}

TEST(UnorderedSet, Emplace) {
    unordered_set set;

    auto [new_it, new_inserted] = set.emplace(1);
    EXPECT_EQ(*new_it, 1);
    EXPECT_TRUE(new_inserted);
    EXPECT_EQ(set.size(), 1);

    auto [existing_it, duplicate_inserted] = set.emplace(1);
    EXPECT_EQ(existing_it, new_it);
    EXPECT_FALSE(duplicate_inserted);
    EXPECT_EQ(set.size(), 1);
}

TEST(UnorderedSet, Insert) {
    unordered_set set;

    int val = 1;
    set.insert(std::move(val));

    auto [_, success] = set.insert(1);
    EXPECT_FALSE(success);

    set.insert({1, 2});
    EXPECT_EQ(set.size(), 2);
}

TEST(UnorderedSet, Find) {
    unordered_set set;
    set.emplace(1);

    auto it = set.find(1);
    EXPECT_EQ(*it, 1);

    EXPECT_EQ(set.find(2), set.end());
}

TEST(UnorderedSet, Contains) {
    unordered_set set;
    set.emplace(1);

    EXPECT_TRUE(set.contains(1));
    EXPECT_FALSE(set.contains(2));
}

TEST(UnorderedSet, EqualRange) {
    unordered_set set;
    set.emplace(1);

    auto [first, last] = set.equal_range(1);
    EXPECT_EQ(first, set.find(1));
    EXPECT_EQ(last, set.end());
}

TEST(UnorderedSet, Count) {
    unordered_set set;
    set.emplace(1);

    EXPECT_EQ(set.count(1), 1);
    EXPECT_EQ(set.count(2), 0);
}
