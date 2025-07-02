#include "type_traits.hpp"
#include "utility.hpp"

#include <gtest/gtest.h>

TEST(Utility, Forwarding) {
    int lvalue = 1;

    EXPECT_TRUE((mystd::is_same_v<int &, decltype(mystd::forward<int &>(lvalue))>));
    EXPECT_TRUE((mystd::is_same_v<int &&, decltype(mystd::forward<int>(lvalue))>));
    EXPECT_TRUE((mystd::is_same_v<int &&, decltype(mystd::forward<int>(1 + lvalue))>));
}

TEST(Utility, Move) {
    int lvalue = 1;

    EXPECT_TRUE((mystd::is_same_v<int &&, decltype(mystd::move(lvalue))>));
    EXPECT_TRUE((mystd::is_same_v<int &&, decltype(mystd::move(32))>));
}
