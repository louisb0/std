#include "type_traits.hpp"
#include "unordered_multiset.hpp"
#include "unordered_set.hpp"
#include "vector.hpp"

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

TEST(UnorderedMultiSet, EqualRange) {
    struct FirstBucketHash {
        size_t operator()(int) const noexcept { return 0; }
    };
    mystd::unordered_multiset<int, FirstBucketHash> set;
    set.emplace(1);
    set.emplace(2);
    set.emplace(2);
    set.emplace(2);
    set.emplace(3);

    auto [first, last] = set.equal_range(2);
    EXPECT_EQ(mystd::distance(first, last), 3);

    for (auto it = first; it != last; ++it) {
        EXPECT_EQ(*it, 2);
    }
}

TEST(UnorderedMultiSet, Count) {
    mystd::unordered_multiset<int> set;
    set.emplace(1);
    set.emplace(1);

    EXPECT_EQ(set.count(1), 2);
    EXPECT_EQ(set.count(2), 0);
}

TEST(UnorderedMultiSet, ElementOrdering) {
    struct FirstBucketHash {
        size_t operator()(int) const noexcept { return 0; }
    };
    mystd::unordered_multiset<int, FirstBucketHash> set;
    set.emplace(1);
    set.emplace(2);
    set.emplace(1);

    mystd::unordered_set<int> seen_and_finished;

    for (auto it = set.begin(); it != set.end(); ++it) {
        int current = *it;
        EXPECT_EQ(seen_and_finished.count(current), 0);

        auto next_it = mystd::next(it);
        bool continues = (next_it != set.end()) && (*next_it == current);
        if (!continues) {
            seen_and_finished.emplace(current);
        }
    }
}
