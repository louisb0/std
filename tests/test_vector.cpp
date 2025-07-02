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
    mystd::vector<int> second(mystd::move(first));

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

TEST(Vector, Reserve) {
    mystd::vector<int> pod_vec = {1, 2, 3};

    // POD happy-path
    pod_vec.reserve(10);
    EXPECT_EQ(pod_vec[0], 1);
    EXPECT_EQ(pod_vec[1], 2);
    EXPECT_EQ(pod_vec[2], 3);
    EXPECT_EQ(pod_vec.size(), 3);
    EXPECT_EQ(pod_vec.capacity(), 10);

    // Redundant reserve
    int *prev_pod_data = pod_vec.data();
    pod_vec.reserve(5);
    EXPECT_EQ(pod_vec.data(), prev_pod_data);
    EXPECT_EQ(pod_vec.size(), 3);
    EXPECT_EQ(pod_vec.capacity(), 10);

    // Moves when noexcept
    struct MoveNoThrow {
        bool was_move_constructed = false;
        MoveNoThrow() = default;
        MoveNoThrow(MoveNoThrow &&other) noexcept : was_move_constructed(true) {}
        MoveNoThrow(const MoveNoThrow &other) : was_move_constructed(false) {}
    };
    mystd::vector<MoveNoThrow> move_vec(3);
    move_vec.reserve(10);
    for (const auto &elem : move_vec) {
        EXPECT_TRUE(elem.was_move_constructed);
    }

    // Copy when unable to safely move
    struct MoveThrows {
        bool was_copy_constructed = false;
        MoveThrows() = default;
        MoveThrows(MoveThrows &&other) : was_copy_constructed(false) {}
        MoveThrows(const MoveThrows &other) : was_copy_constructed(true) {}
    };
    mystd::vector<MoveThrows> copy_vec(3);
    copy_vec.reserve(10);
    for (const auto &elem : copy_vec) {
        EXPECT_TRUE(elem.was_copy_constructed);
    }
}

TEST(Vector, Clear) {
    mystd::vector<int> vec = {1, 2, 3};
    vec.clear();

    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.capacity(), 3);
    EXPECT_TRUE(vec.empty());
}

TEST(Vector, PopBack) {
    mystd::vector<int> vec = {1, 2, 3};
    vec.pop_back();

    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec.capacity(), 3);
}

TEST(Vector, EmplaceAndPushBack) {
    struct ConstructTracker {
        bool defaulted = false;
        bool copied = false;
        bool moved = false;

        ConstructTracker() : defaulted(true) {}
        ConstructTracker(const ConstructTracker &other) : copied(true) {}
        ConstructTracker(ConstructTracker &&other) : moved(true) {}
    };
    mystd::vector<ConstructTracker> vec;

    auto &tracker = vec.emplace_back();
    EXPECT_TRUE(tracker.defaulted);

    vec.push_back(ConstructTracker{});
    EXPECT_TRUE(vec.back().moved);

    ConstructTracker lval{};
    vec.push_back(lval);
    EXPECT_TRUE(vec.back().copied);
}
