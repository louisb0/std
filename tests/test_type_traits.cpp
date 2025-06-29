#include "type_traits.hpp"

#include <gtest/gtest.h>

TEST(TypeTraits, IsSameValue) {
    EXPECT_TRUE((mystd::is_same_v<int, int>));
    EXPECT_FALSE((mystd::is_same_v<int, float>));
}

TEST(TypeTraits, IsReference) {
    EXPECT_FALSE((mystd::is_reference_v<int>));
    EXPECT_TRUE((mystd::is_reference_v<int &>));
    EXPECT_TRUE((mystd::is_reference_v<int &&>));
}

// https://en.cppreference.com/w/cpp/meta.html#Definitions
TEST(TypeTraits, IsReferenceable) {
    EXPECT_TRUE(mystd::is_referenceable_v<int>);
    EXPECT_TRUE(mystd::is_referenceable_v<const int>);
    EXPECT_TRUE(mystd::is_referenceable_v<volatile int>);
    EXPECT_TRUE(mystd::is_referenceable_v<int *>);
    EXPECT_TRUE(mystd::is_referenceable_v<int[5]>);
    EXPECT_TRUE(mystd::is_referenceable_v<int (*)()>);
    EXPECT_FALSE(mystd::is_referenceable_v<void>);
    EXPECT_FALSE(mystd::is_referenceable_v<const void>);
    EXPECT_FALSE(mystd::is_referenceable_v<volatile void>);

    EXPECT_TRUE(mystd::is_referenceable_v<int()>);
    EXPECT_FALSE(mystd::is_referenceable_v<int() const>);
    EXPECT_FALSE(mystd::is_referenceable_v<int() volatile>);
    EXPECT_FALSE(mystd::is_referenceable_v<int() const volatile>);
    EXPECT_FALSE(mystd::is_referenceable_v<int() &>);
    EXPECT_FALSE(mystd::is_referenceable_v<int() &&>);

    EXPECT_TRUE(mystd::is_referenceable_v<int &>);
    EXPECT_TRUE(mystd::is_referenceable_v<int (&)()>);
    EXPECT_TRUE(mystd::is_referenceable_v<const int &>);
    EXPECT_TRUE(mystd::is_referenceable_v<int &&>);
}
