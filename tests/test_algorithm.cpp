#include "algorithm.hpp"

#include <gtest/gtest.h>

struct Moveable {
    bool moved_to = false;

    Moveable() = default;
    Moveable &operator=(Moveable &&other) {
        moved_to = true;
        return *this;
    }
};

TEST(Algorithm, Move) {
    Moveable src[5];
    Moveable dest[5];

    auto it = mystd::move(src, src + 4, dest);
    EXPECT_FALSE(it->moved_to);

    for (int i = 0; i < 4; i++) {
        EXPECT_FALSE(src[i].moved_to);
        EXPECT_TRUE(dest[i].moved_to);
    }
}

TEST(Algorithm, MoveBackward) {
    Moveable src[5];
    Moveable dest[5];

    auto it = mystd::move_backward(src, src + 4, dest + 4);
    EXPECT_TRUE(it->moved_to);

    for (int i = 0; i < 4; i++) {
        EXPECT_FALSE(src[i].moved_to);
        EXPECT_TRUE(dest[i].moved_to);
    }
    EXPECT_FALSE(dest[4].moved_to);
}

struct Copyable {
    bool is_copy = false;

    Copyable() = default;
    Copyable &operator=(const Copyable &other) {
        is_copy = true;
        return *this;
    }
};

TEST(Algorithm, Copy) {
    Copyable src[5];
    Copyable dest[5];

    auto it = mystd::copy(src, src + 4, dest);
    EXPECT_FALSE(it->is_copy);

    for (int i = 0; i < 4; i++) {
        EXPECT_FALSE(src[i].is_copy);
        EXPECT_TRUE(dest[i].is_copy);
    }
}
