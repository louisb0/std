#include "initializer_list.hpp"

#include <gtest/gtest.h>

TEST(InitializerList, EmptyListProperties) {
    mystd::initializer_list<int> empty_il = {};
    EXPECT_EQ(empty_il.size(), 0);
    EXPECT_EQ(empty_il.begin(), empty_il.end());
}

TEST(InitializerList, StdCompatibility) {
    std::initializer_list<int> std_il = {1, 2, 3, 4, 5};
    mystd::initializer_list<int> from_std = std_il;

    EXPECT_EQ(from_std.size(), std_il.size());
    EXPECT_TRUE(std::equal(from_std.begin(), from_std.end(), std_il.begin()));

    EXPECT_NE(from_std.begin(), std_il.begin());
    EXPECT_NE(from_std.end(), std_il.end());
}

TEST(InitializerList, Iterators) {
    mystd::initializer_list<int> il = {1, 2, 3, 4, 5};

    EXPECT_EQ(il.begin(), mystd::begin(il));
    EXPECT_EQ(il.end(), mystd::end(il));
    // TODO: EXPECT_TRUE(mystd::distance(il.begin(), il.end(), il.size()));

    auto begin = mystd::begin(il);
    EXPECT_EQ(*begin, 1);
    EXPECT_EQ(*(++begin), 2);

    auto rbegin = mystd::rbegin(il);
    EXPECT_EQ(*rbegin, 5);
    EXPECT_EQ(*(++rbegin), 4);
    EXPECT_NE(rbegin, mystd::rend(il));
}

TEST(InitializerList, SizeLimit) {
    EXPECT_THROW(
        {
            mystd::initializer_list<int> oversized_il(
                {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17});
        },
        std::length_error);

    EXPECT_NO_THROW({
        mystd::initializer_list<int> max_size_il(
            {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
    });
}
