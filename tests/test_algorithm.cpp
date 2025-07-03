#include "algorithm.hpp"

#include <gtest/gtest.h>

struct Moveable {
    bool moved_from = false;

    Moveable() = default;
    Moveable &operator=(Moveable &&other) {
        other.moved_from = true;
        return *this;
    }
};

TEST(Algorithm, Move) {
    Moveable src[5];
    Moveable dest[5];

    auto it = mystd::move(src, src + 4, dest);
    EXPECT_FALSE(it->moved_from);

    for (int i = 0; i < 4; i++) {
        EXPECT_TRUE(src[i].moved_from);
        EXPECT_FALSE(dest[i].moved_from);
    }
}

TEST(Algorithm, Backward) {
    Moveable src[5];
    Moveable dest[5];

    auto it = mystd::move_backward(src, src + 4, dest + 4);
    EXPECT_FALSE(it->moved_from);

    for (int i = 0; i < 4; i++) {
        EXPECT_TRUE(src[i].moved_from);
        EXPECT_FALSE(dest[i].moved_from);
    }
}
