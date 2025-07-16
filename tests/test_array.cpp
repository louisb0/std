#include "array.hpp"
#include "iterator.hpp"

#include <gtest/gtest.h>

TEST(Array, DefaultConstructor) {
    mystd::array<int, 3> arr;

    int *data = arr.data();
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[1], 0);

    EXPECT_EQ(arr.size(), 3);
    EXPECT_FALSE(arr.empty());
}

TEST(Array, InitializerListConstructor) {
    mystd::array<int, 3> arr{1, 2, 3};

    int *data = arr.data();
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[1], 2);

    EXPECT_EQ(arr.size(), 3);
    EXPECT_FALSE(arr.empty());
}

TEST(Array, Comparison) {
    mystd::array<int, 3> vec1 = {1, 2, 3};
    mystd::array<int, 3> vec2 = {1, 2, 3};
    mystd::array<int, 3> vec3 = {1, 2, 4};

    EXPECT_TRUE(vec1 == vec2);
    EXPECT_TRUE(vec1 != vec3);

    EXPECT_TRUE(vec1 < vec3);
    EXPECT_TRUE(vec3 > vec1);

    EXPECT_TRUE(vec1 <= vec2);
    EXPECT_TRUE(vec1 >= vec2);
}

TEST(Array, InitializerListAssignment) {
    mystd::array<int, 3> arr;

    arr = {1, 2, 3};
    EXPECT_EQ(arr, (mystd::array<int, 3>{1, 2, 3}));
}

TEST(Array, ElementAccess) {
    mystd::array<int, 2> arr = {1, 2};

    arr[0] = 10;
    EXPECT_EQ(arr[0], 10);
    EXPECT_EQ(arr[1], 2);

    arr.at(1) = 20;
    EXPECT_EQ(arr.at(0), 10);
    EXPECT_EQ(arr.at(1), 20);
    EXPECT_THROW(arr.at(-1), std::out_of_range);
    EXPECT_THROW(arr.at(3), std::out_of_range);
    EXPECT_THROW(arr.at(3) = 100, std::out_of_range);

    EXPECT_EQ(arr.front(), 10);
    EXPECT_EQ(arr.back(), 20);
    arr.front() = 100;
    arr.back() = 200;
    EXPECT_EQ(arr.front(), 100);
    EXPECT_EQ(arr.back(), 200);
}

TEST(Array, Iterators) {
    mystd::array<int, 3> arr = {1, 2, 3};

    EXPECT_EQ(mystd::distance(arr.begin(), arr.end()), 3);
    EXPECT_EQ(mystd::distance(arr.rbegin(), arr.rend()), 3);

    EXPECT_EQ(arr.begin(), arr.cbegin());
    EXPECT_EQ(arr.end(), arr.cend());
    EXPECT_EQ(arr.rbegin(), arr.crbegin());
    EXPECT_EQ(arr.rend(), arr.crend());

    std::array<int, 3> expected_forward = {1, 2, 3};
    EXPECT_TRUE(std::equal(arr.begin(), arr.end(), expected_forward.begin()));

    std::array<int, 3> expected_reverse = {3, 2, 1};
    EXPECT_TRUE(std::equal(arr.rbegin(), arr.rend(), expected_reverse.begin()));
}

TEST(Array, Fill) {
    mystd::array<int, 3> arr;

    arr.fill(5);
    EXPECT_EQ(arr, (mystd::array<int, 3>{5, 5, 5}));
}

TEST(Array, Swap) {
    mystd::array<int, 3> arr1{1, 2, 3};
    mystd::array<int, 3> arr2{4, 5, 6};

    arr1.swap(arr2);
    EXPECT_EQ(arr1, (mystd::array<int, 3>{4, 5, 6}));
    EXPECT_EQ(arr2, (mystd::array<int, 3>{1, 2, 3}));

    mystd::swap(arr1, arr2);
    EXPECT_EQ(arr1, (mystd::array<int, 3>{1, 2, 3}));
    EXPECT_EQ(arr2, (mystd::array<int, 3>{4, 5, 6}));
}
