#include "vector.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

TEST(Vector, DefaultConstructor) {
    mystd::vector<int> defaulted;

    EXPECT_EQ(defaulted.data(), nullptr);
    EXPECT_EQ(defaulted.size(), 0);
    EXPECT_EQ(defaulted.capacity(), 0);
    EXPECT_TRUE(defaulted.empty());
}

TEST(Vector, CountConstructor) {
    mystd::vector<int> count(3);

    int *data = count.data();
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[0], 0);

    EXPECT_EQ(count.size(), 3);
    EXPECT_EQ(count.capacity(), 3);
    EXPECT_FALSE(count.empty());
}

TEST(Vector, CountValueConstructor) {
    mystd::vector<int> count(3, 3);

    int *data = count.data();
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[0], 3);

    EXPECT_EQ(count.size(), 3);
    EXPECT_EQ(count.capacity(), 3);
    EXPECT_FALSE(count.empty());
}

TEST(Vector, InitializerListConstructor) {
    mystd::vector<int> count = {1, 2, 3};

    int *data = count.data();
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[2], 3);

    EXPECT_EQ(count.size(), 3);
    EXPECT_EQ(count.capacity(), 3);
    EXPECT_FALSE(count.empty());
}

TEST(Vector, CopyConstructor) {
    mystd::vector<int> first = {1, 2, 3};
    mystd::vector<int> copy(first);

    EXPECT_NE(first.data(), copy.data());
    EXPECT_EQ(first.data()[2], copy.data()[2]);

    EXPECT_EQ(first.size(), copy.size());
    EXPECT_EQ(first.capacity(), copy.capacity());
    EXPECT_EQ(first.empty(), copy.empty());
}

TEST(Vector, MoveConstructor) {
    mystd::vector<int> first = {1, 2, 3};
    int *store = first.data();
    mystd::vector<int> second(std::move(first));

    EXPECT_EQ(first.data(), nullptr);
    EXPECT_EQ(first.size(), 0);
    EXPECT_EQ(first.capacity(), 0);
    EXPECT_TRUE(first.empty());

    EXPECT_EQ(second.data(), store);
    EXPECT_EQ(second.size(), 3);
    EXPECT_EQ(second.capacity(), 3);
    EXPECT_FALSE(second.empty());
}

TEST(Vector, ElementAccess) {
    mystd::vector<int> vec = {1, 2};

    vec[0] = 10;
    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[1], 2);

    vec.at(1) = 20;
    EXPECT_EQ(vec.at(0), 10);
    EXPECT_EQ(vec.at(1), 20);

    EXPECT_THROW(vec.at(-1), std::out_of_range);
    EXPECT_THROW(vec.at(3), std::out_of_range);
    EXPECT_THROW(vec.at(3) = 100, std::out_of_range);

    EXPECT_EQ(vec.front(), 10);
    EXPECT_EQ(vec.back(), 20);
    vec.front() = 100;
    vec.back() = 200;
    EXPECT_EQ(vec.front(), 100);
    EXPECT_EQ(vec.back(), 200);

    int *data = vec.data();
    EXPECT_EQ(data[0], 100);
    EXPECT_EQ(data[1], 200);
}

TEST(Vector, Iterators) {
    mystd::vector<int> vec = {1, 2, 3};

    EXPECT_EQ(mystd::distance(vec.begin(), vec.end()), 3);
    EXPECT_EQ(mystd::distance(vec.rbegin(), vec.rend()), 3);

    EXPECT_EQ(vec.begin(), vec.cbegin());
    EXPECT_EQ(vec.end(), vec.cend());
    EXPECT_EQ(vec.rbegin(), vec.crbegin());
    EXPECT_EQ(vec.rend(), vec.crend());

    int expected = 1;
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        EXPECT_EQ(*it, expected++);
    }

    expected = 3;
    for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
        EXPECT_EQ(*it, expected--);
    }

    expected = 1;
    for (const auto &elem : vec) {
        EXPECT_EQ(elem, expected++);
    }
}

// TEST(Vector, Resize) {
//     mystd::vector<int> x = {1, 2, 3};
//
//     x.reserve(10);
//     EXPECT_EQ(x[0], 1);
//     EXPECT_EQ(x[1], 2);
//     EXPECT_EQ(x[2], 3);
//
//     EXPECT_EQ(x.size(), 3);
//     EXPECT_EQ(x.capacity(), 10);
// }
