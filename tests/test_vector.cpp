#include "vector.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

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
    static constexpr uint8_t defaulted = 0b00000001;
    static constexpr uint8_t copied = 0b00000010;
    static constexpr uint8_t moved = 0b00000100;

    struct ConstructTracker {
        uint8_t flags = 0;

        ConstructTracker() { flags |= defaulted; }
        ConstructTracker(const ConstructTracker &other) : flags(other.flags) { flags |= copied; }
        ConstructTracker(ConstructTracker &&other) : flags(other.flags) { flags |= moved; }
    };

    mystd::vector<ConstructTracker> vec;

    auto &tracker = vec.emplace_back();
    EXPECT_EQ(tracker.flags, defaulted);

    vec.push_back(ConstructTracker{});
    EXPECT_EQ(vec.back().flags, defaulted | moved);

    ConstructTracker lval{};
    vec.push_back(lval);
    EXPECT_EQ(vec.back().flags, defaulted | copied);
}

TEST(Vector, ErasePoint) {
    mystd::vector<int> vec = {1, 2, 3, 4};

    // Middle
    auto it = vec.erase(vec.begin() + 1); // {1, 3, 4}
    EXPECT_EQ(*it, 3);

    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 3);
    EXPECT_EQ(vec[2], 4);
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec.capacity(), 4);

    // Start
    it = vec.erase(vec.begin()); // {3, 4}
    EXPECT_EQ(*it, 3);

    EXPECT_EQ(vec[0], 3);
    EXPECT_EQ(vec[1], 4);
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec.capacity(), 4);

    // End
    it = vec.erase(vec.end() - 1); // {3}
    EXPECT_EQ(it, vec.end());

    EXPECT_EQ(vec[0], 3);
    EXPECT_EQ(vec.size(), 1);
    EXPECT_EQ(vec.capacity(), 4);
}

TEST(Vector, EraseRange) {
    mystd::vector<int> vec = {1, 2, 3, 4};

    // Subset
    auto it = vec.erase(vec.begin() + 1, vec.end() - 1); // {1, 4}
    EXPECT_EQ(*it, 4);

    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 4);
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec.capacity(), 4);

    // Full
    it = vec.erase(vec.begin(), vec.end()); // {}
    EXPECT_EQ(it, vec.end());

    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.capacity(), 4);

    // Empty
    it = vec.erase(vec.begin(), vec.end()); // {}
    EXPECT_EQ(it, vec.end());

    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.capacity(), 4);
}

TEST(Vector, Emplace) {
    mystd::vector<std::string> vec;

    auto it = vec.emplace(vec.begin(), "a");
    EXPECT_EQ(*it, "a");
    EXPECT_EQ(vec.size(), 1);

    it = vec.emplace(vec.end(), "c");
    EXPECT_EQ(*it, "c");
    EXPECT_EQ(vec.size(), 2);

    it = vec.emplace(vec.begin() + 1, "b");
    EXPECT_EQ(*it, "b");
    EXPECT_EQ(vec.size(), 3);

    EXPECT_EQ(vec[0], "a");
    EXPECT_EQ(vec[1], "b");
    EXPECT_EQ(vec[2], "c");
}

TEST(Vector, InsertSingle) {
    mystd::vector<int> vec;

    auto it = vec.insert(vec.begin(), 1);
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(vec.size(), 1);

    it = vec.insert(vec.end(), 3);
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(vec.size(), 2);

    int to_move = 2;
    it = vec.insert(vec.begin() + 1, std::move(2));
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(vec.size(), 3);

    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
    EXPECT_EQ(vec[2], 3);
}

TEST(Vector, InsertCount) {
    mystd::vector<int> vec;

    auto it = vec.insert(vec.begin(), 2, 1);
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(vec.size(), 2);

    it = vec.insert(vec.end(), 2, 3);
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(vec.size(), 4);

    int to_move = 2;
    it = vec.insert(vec.begin() + 2, 2, std::move(2));
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(vec.size(), 6);

    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 1);
    EXPECT_EQ(vec[2], 2);
    EXPECT_EQ(vec[3], 2);
    EXPECT_EQ(vec[4], 3);
    EXPECT_EQ(vec[5], 3);
}

TEST(Vector, InsertIterators) {
    mystd::vector<int> to;

    mystd::vector<int> empty;
    to.insert(to.begin(), empty.begin(), empty.end());
    EXPECT_EQ(to.size(), 0);

    mystd::vector<int> first = {1, 2};
    auto it = to.insert(to.begin(), first.begin(), first.end());
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(to.size(), 2);

    mystd::vector<int> second = {4, 5};
    it = to.insert(to.end(), second.begin(), second.end());
    EXPECT_EQ(*it, 4);
    EXPECT_EQ(to.size(), 4);

    mystd::vector<int> third = {3};
    it = to.insert(to.begin() + 2, third.begin(), third.end());
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(to.size(), 5);

    EXPECT_EQ(to[0], 1);
    EXPECT_EQ(to[1], 2);
    EXPECT_EQ(to[2], 3);
    EXPECT_EQ(to[3], 4);
    EXPECT_EQ(to[4], 5);
}
