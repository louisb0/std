#include "vector.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

TEST(Vector, DefaultConstructor) {
    mystd::vector<int> vec;

    EXPECT_EQ(vec.data(), nullptr);
    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.capacity(), 0);
    EXPECT_TRUE(vec.empty());
}

TEST(Vector, CountConstructor) {
    mystd::vector<int> vec(3);

    int *data = vec.data();
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[0], 0);

    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec.capacity(), 3);
    EXPECT_FALSE(vec.empty());
}

TEST(Vector, CountValueConstructor) {
    mystd::vector<int> vec(3, 3);

    int *data = vec.data();
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[0], 3);

    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec.capacity(), 3);
    EXPECT_FALSE(vec.empty());
}

TEST(Vector, InitializerListConstructor) {
    mystd::vector<int> vec = {1, 2, 3};

    int *data = vec.data();
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[0], 1);

    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec.capacity(), 3);
    EXPECT_FALSE(vec.empty());
}

TEST(Vector, CopyConstructor) {
    mystd::vector<int> orig = {1, 2, 3};
    mystd::vector<int> copy_constructed(orig);

    EXPECT_NE(orig.data(), copy_constructed.data());
    EXPECT_EQ(orig.data()[0], copy_constructed.data()[0]);

    EXPECT_EQ(orig.size(), copy_constructed.size());
    EXPECT_EQ(orig.capacity(), copy_constructed.capacity());
    EXPECT_EQ(orig.empty(), copy_constructed.empty());
}

TEST(Vector, MoveConstructor) {
    mystd::vector<int> orig = {1, 2, 3};
    int *data_location = orig.data();
    mystd::vector<int> move_constructed(mystd::move(orig));

    EXPECT_EQ(orig.data(), nullptr);
    EXPECT_EQ(orig.size(), 0);
    EXPECT_EQ(orig.capacity(), 0);
    EXPECT_TRUE(orig.empty());

    EXPECT_EQ(move_constructed.data(), data_location);
    EXPECT_EQ(move_constructed.size(), 3);
    EXPECT_EQ(move_constructed.capacity(), 3);
    EXPECT_FALSE(move_constructed.empty());
}

TEST(Vector, CopyAssignment) {
    // Copy constructor proxy
    {
        mystd::vector<int> vec1 = {3};
        mystd::vector<int> vec2 = {1, 2};
        EXPECT_LT(vec1.capacity(), vec2.size());

        vec1 = vec2;
        EXPECT_EQ(vec1, vec2);
    }

    // Copy initialized + destroy
    {
        mystd::vector<int> vec1 = {1, 2};
        mystd::vector<int> vec2 = {3};

        EXPECT_LE(vec2.size(), vec1.capacity());
        EXPECT_GT(vec1.size(), std::min(vec1.size(), vec2.size()));

        vec1 = vec2;
        EXPECT_EQ(vec1, vec2);
    }

    // Copy initialized + uninitialized
    {
        mystd::vector<int> vec1 = {3};
        vec1.reserve(5);
        mystd::vector<int> vec2 = {1, 2};

        EXPECT_LE(vec2.size(), vec1.capacity());
        EXPECT_GT(vec2.size(), std::min(vec1.size(), vec2.size()));

        vec1 = vec2;
        EXPECT_EQ(vec1, vec2);
    }
}

TEST(Vector, MoveAssignment) {
    mystd::vector<int> vec1 = {1, 2};
    mystd::vector<int> vec2 = {3};

    vec1 = std::move(vec2);
    EXPECT_EQ(vec1, (mystd::vector<int>{3}));
}

TEST(Vector, InitializerListAssignment) {
    mystd::vector<int> vec;

    vec = {1, 2, 3};
    EXPECT_EQ(vec, (mystd::vector<int>{1, 2, 3}));
}

TEST(Vector, Comparison) {
    mystd::vector<int> vec1 = {1, 2, 3};
    mystd::vector<int> vec2 = {1, 2, 3};
    mystd::vector<int> vec3 = {1, 2, 4};
    mystd::vector<int> vec4 = {1, 2};

    EXPECT_TRUE(vec1 == vec2);
    EXPECT_TRUE(vec1 != vec3);

    EXPECT_TRUE(vec1 < vec3);
    EXPECT_TRUE(vec3 > vec1);

    EXPECT_TRUE(vec4 < vec3);
    EXPECT_TRUE(vec3 > vec4);

    EXPECT_TRUE(vec1 <= vec2);
    EXPECT_TRUE(vec1 >= vec2);
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
}

TEST(Vector, Iterators) {
    mystd::vector<int> vec = {1, 2, 3};

    EXPECT_EQ(mystd::distance(vec.begin(), vec.end()), 3);
    EXPECT_EQ(mystd::distance(vec.rbegin(), vec.rend()), 3);

    EXPECT_EQ(vec.begin(), vec.cbegin());
    EXPECT_EQ(vec.end(), vec.cend());
    EXPECT_EQ(vec.rbegin(), vec.crbegin());
    EXPECT_EQ(vec.rend(), vec.crend());

    std::vector<int> expected_forward = {1, 2, 3};
    EXPECT_TRUE(std::equal(vec.begin(), vec.end(), expected_forward.begin()));

    std::vector<int> expected_reverse = {3, 2, 1};
    EXPECT_TRUE(std::equal(vec.rbegin(), vec.rend(), expected_reverse.begin()));
}

TEST(Vector, Reserve) {
    mystd::vector<int> vec = {1, 2, 3};

    vec.reserve(10);
    EXPECT_EQ(vec, (mystd::vector<int>{1, 2, 3}));
    EXPECT_EQ(vec.capacity(), 10);

    vec.reserve(5);
    EXPECT_EQ(vec, (mystd::vector<int>{1, 2, 3}));
    EXPECT_EQ(vec.capacity(), 10);
}

TEST(Vector, ShrinkToFit) {
    mystd::vector<int> vec = {1, 2};
    vec.push_back(3);
    EXPECT_EQ(vec.capacity(), 4);

    vec.shrink_to_fit();
    EXPECT_EQ(vec.capacity(), 3);
    EXPECT_EQ(vec, (mystd::vector<int>{1, 2, 3}));
}

TEST(Vector, Emplace) {
    mystd::vector<std::string> vec;

    auto it = vec.emplace(vec.begin(), "a");
    EXPECT_EQ(*it, "a");
    EXPECT_EQ(vec, (mystd::vector<std::string>{"a"}));

    it = vec.emplace(vec.end(), "c");
    EXPECT_EQ(*it, "c");
    EXPECT_EQ(vec, (mystd::vector<std::string>{"a", "c"}));

    it = vec.emplace(vec.begin() + 1, "b");
    EXPECT_EQ(*it, "b");
    EXPECT_EQ(vec, (mystd::vector<std::string>{"a", "b", "c"}));
}

TEST(Vector, EmplaceBackPushBack) {
    struct Tracker {
        enum { defaulted = 0b00000001, copied = 0b00000010, moved = 0b00000100 };
        uint8_t flags = 0;

        Tracker() { flags |= defaulted; }
        Tracker(const Tracker &other) : flags(other.flags) { flags |= copied; }
        Tracker(Tracker &&other) : flags(other.flags) { flags |= moved; }
    };
    mystd::vector<Tracker> vec;

    EXPECT_EQ(vec.emplace_back().flags, Tracker::defaulted);

    Tracker lvalue{};
    vec.push_back(lvalue);
    EXPECT_EQ(vec.back().flags, Tracker::defaulted | Tracker::copied);

    vec.push_back(Tracker{});
    EXPECT_EQ(vec.back().flags, Tracker::defaulted | Tracker::moved);
}

TEST(Vector, PopBack) {
    mystd::vector<int> vec = {1, 2, 3};

    vec.pop_back();
    EXPECT_EQ(vec, (mystd::vector<int>{1, 2}));
}

TEST(Vector, InsertSingle) {
    mystd::vector<int> vec;

    auto it = vec.insert(vec.begin(), 1);
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(vec, (mystd::vector<int>{1}));

    it = vec.insert(vec.end(), 3);
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(vec, (mystd::vector<int>{1, 3}));

    int to_move = 2;
    it = vec.insert(vec.begin() + 1, std::move(to_move));
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(vec, (mystd::vector<int>{1, 2, 3}));
}

TEST(Vector, InsertCount) {
    mystd::vector<int> vec;

    auto it = vec.insert(vec.begin(), 2, 1);
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(vec, (mystd::vector<int>{1, 1}));

    it = vec.insert(vec.end(), 2, 3);
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(vec, (mystd::vector<int>{1, 1, 3, 3}));

    int to_move = 2;
    it = vec.insert(vec.begin() + 2, 2, std::move(2));
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(vec, (mystd::vector<int>{1, 1, 2, 2, 3, 3}));
}

TEST(Vector, InsertIterators) {
    mystd::vector<int> to;

    mystd::vector<int> empty;
    to.insert(to.begin(), empty.begin(), empty.end());
    EXPECT_EQ(to.size(), 0);

    mystd::vector<int> first = {1, 2};
    auto it = to.insert(to.begin(), first.begin(), first.end());
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(to, (mystd::vector<int>{1, 2}));

    mystd::vector<int> second = {4, 5};
    it = to.insert(to.end(), second.begin(), second.end());
    EXPECT_EQ(*it, 4);
    EXPECT_EQ(to, (mystd::vector<int>{1, 2, 4, 5}));

    mystd::vector<int> third = {3};
    it = to.insert(to.begin() + 2, third.begin(), third.end());
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(to, (mystd::vector<int>{1, 2, 3, 4, 5}));
}

TEST(Vector, ErasePoint) {
    mystd::vector<int> vec = {1, 2, 3, 4};
    EXPECT_EQ(vec.capacity(), 4);

    auto it = vec.erase(vec.begin() + 1);
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(vec, (mystd::vector<int>{1, 3, 4}));

    it = vec.erase(vec.begin());
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(vec, (mystd::vector<int>{3, 4}));

    it = vec.erase(vec.end() - 1);
    EXPECT_EQ(it, vec.end());
    EXPECT_EQ(vec, (mystd::vector<int>{3}));

    EXPECT_EQ(vec.capacity(), 4);
}

TEST(Vector, EraseRange) {
    mystd::vector<int> vec = {1, 2, 3, 4};
    EXPECT_EQ(vec.capacity(), 4);

    auto it = vec.erase(vec.begin() + 1, vec.end() - 1);
    EXPECT_EQ(*it, 4);
    EXPECT_EQ(vec, (mystd::vector<int>{1, 4}));

    it = vec.erase(vec.begin(), vec.end());
    EXPECT_EQ(it, vec.end());
    EXPECT_EQ(vec, (mystd::vector<int>{}));

    it = vec.erase(vec.begin(), vec.end());
    EXPECT_EQ(it, vec.end());
    EXPECT_EQ(vec, (mystd::vector<int>{}));

    EXPECT_EQ(vec.capacity(), 4);
}

TEST(Vector, Clear) {
    mystd::vector<int> vec = {1, 2, 3};

    vec.clear();
    EXPECT_EQ(vec, (mystd::vector<int>{}));
}

TEST(Vector, Resize) {
    mystd::vector<int> vec = {1, 2, 3};

    vec.resize(2);
    EXPECT_EQ(vec, (mystd::vector<int>{1, 2}));

    vec.resize(5, 42);
    EXPECT_EQ(vec, (mystd::vector<int>{1, 2, 42, 42, 42}));

    vec.resize(5, 99);
    EXPECT_EQ(vec, (mystd::vector<int>{1, 2, 42, 42, 42}));
}

TEST(Vector, Swap) {
    mystd::vector<int> a = {1};
    mystd::vector<int> b = {2, 3};

    a.swap(b);
    EXPECT_EQ(a.size(), 2);
    EXPECT_EQ(b.size(), 1);

    mystd::swap(a, b);
    EXPECT_EQ(a.size(), 1);
    EXPECT_EQ(b.size(), 2);
}
