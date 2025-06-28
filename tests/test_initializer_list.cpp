#include "initializer_list.hpp"

#include <gtest/gtest.h>

TEST(InitializerList, EmptyList) {
    mystd::initializer_list<int> empty = {};

    EXPECT_EQ(empty.size(), 0);
    EXPECT_EQ(empty.begin(), empty.end());
}

TEST(InitializerList, StdConsistent) {
    std::initializer_list std = {1, 2, 3, 4, 5};
    mystd::initializer_list<int> mine = std;

    EXPECT_EQ(mine.size(), std.size());
    EXPECT_NE(mine.begin(), std.begin());
    EXPECT_NE(mine.end(), std.end());

    EXPECT_TRUE(std::equal(mine.begin(), mine.end(), std.begin()));
}

TEST(InitializerList, NonMemberIteratorOveroads) {
    mystd::initializer_list il = {1, 2, 3, 4, 5};

    EXPECT_EQ(il.begin(), mystd::begin(il));
    EXPECT_EQ(il.end(), mystd::end(il));

    auto rbegin = mystd::rbegin(il);
    EXPECT_EQ(*rbegin, 5);
    EXPECT_EQ(*(++rbegin), 4);

    EXPECT_NE(rbegin, mystd::rend(il));
}

TEST(InitializerList, Size) {
    EXPECT_THROW(
        mystd::initializer_list il({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17});
        , std::length_error);
}
