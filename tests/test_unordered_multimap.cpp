#include "type_traits.hpp"
#include "unordered_multimap.hpp"

#include <gtest/gtest.h>

TEST(UnorderedMultiMap, Aliases) {
    using map = mystd::unordered_multimap<const char *, int>;

    EXPECT_TRUE((mystd::is_same_v<map::key_type, const char *>));
    EXPECT_TRUE((mystd::is_same_v<map::mapped_type, int>));
    EXPECT_TRUE((mystd::is_same_v<map::value_type, std::pair<const char *, int>>));
}

TEST(UnorderedMultiMap, Emplace) {
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

TEST(UnorderedMultiMap, Find) {
    mystd::unordered_multimap<const char *, int> map;
    map.emplace("a", 1);

    auto it = map.find("a");
    EXPECT_EQ(it->first, "a");
    EXPECT_EQ(it->second, 1);

    EXPECT_EQ(map.find("NA"), map.end());
}

TEST(UnorderedMultiMap, EqualRange) {
    struct FirstBucketHash {
        size_t operator()(const char *) const noexcept { return 0; }
    };
    mystd::unordered_multimap<const char *, int, FirstBucketHash> map;
    map.emplace("a", 1);
    map.emplace("b", 2);
    map.emplace("b", 3);
    map.emplace("b", 4);
    map.emplace("c", 5);

    auto [first, last] = map.equal_range("b");
    EXPECT_EQ(mystd::distance(first, last), 3);

    for (auto it = first; it != last; ++it) {
        EXPECT_EQ(it->first, "b");
    }
}
TEST(UnorderedMultiMap, Count) {
    mystd::unordered_multimap<const char *, int> map;
    map.emplace("a", 1);
    map.emplace("a", 1);

    EXPECT_EQ(map.count("a"), 2);
    EXPECT_EQ(map.count("b"), 0);
}
