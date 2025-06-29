#include "memory.hpp"

#include <gtest/gtest.h>

#include <string>

TEST(Memory, UninitializedCopyPODType) {
    alignas(int) std::byte buffer[sizeof(int) * 3];
    int src[3] = {1, 2, 3};
    int *dest = reinterpret_cast<int *>(buffer);

    auto it = mystd::uninitialized_copy(src, src + 3, dest);
    EXPECT_EQ(it, dest + 3);

    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(dest[i], src[i]);
    }
}

TEST(Memory, UninitializedCopyClassType) {
    alignas(std::string) std::byte buffer[sizeof(std::string) * 3];
    std::string src[3] = {"hello", "world", "test"};
    std::string *dest = reinterpret_cast<std::string *>(buffer);

    auto it = mystd::uninitialized_copy(src, src + 3, dest);
    EXPECT_EQ(it, dest + 3);

    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(dest[i], src[i]);
    }
}

TEST(Memory, UninitializedDefaultConstruct) {
    alignas(int) std::byte buffer[sizeof(std::string) * 3];
    std::string *dest = reinterpret_cast<std::string *>(buffer);

    mystd::uninitialized_default_construct(dest, dest + 3);
    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(dest[i], "");
    }
}

struct ThrowOnThirdCopy {
    static inline size_t copy_count = 0;

    ThrowOnThirdCopy() = default;
    ThrowOnThirdCopy(const ThrowOnThirdCopy &other) {
        if (copy_count + 1 == 3) {
            throw std::runtime_error("third copy");
        }

        copy_count++;
    }

    ~ThrowOnThirdCopy() { copy_count--; }
};

TEST(Memory, UninitializedCopyThrows) {
    alignas(ThrowOnThirdCopy) std::byte buffer[sizeof(ThrowOnThirdCopy) * 3];
    ThrowOnThirdCopy src[3];
    ThrowOnThirdCopy *dest = reinterpret_cast<ThrowOnThirdCopy *>(buffer);

    EXPECT_EQ(ThrowOnThirdCopy::copy_count, 0);
    EXPECT_THROW({ mystd::uninitialized_copy(src, src + 3, dest); }, std::runtime_error);
    EXPECT_EQ(ThrowOnThirdCopy::copy_count, 0);
}

struct ThrowOnThirdConstruct {
    static inline size_t construct_count = 0;

    ThrowOnThirdConstruct() {
        if (construct_count + 1 == 3) {
            throw std::runtime_error("third construct");
        }

        construct_count++;
    }

    ~ThrowOnThirdConstruct() { construct_count--; }
};

TEST(Memory, UninitializedDefaultconstructThrows) {
    alignas(ThrowOnThirdConstruct) std::byte buffer[sizeof(ThrowOnThirdConstruct) * 3];
    ThrowOnThirdConstruct *dest = reinterpret_cast<ThrowOnThirdConstruct *>(buffer);

    EXPECT_EQ(ThrowOnThirdConstruct::construct_count, 0);
    EXPECT_THROW({ mystd::uninitialized_default_construct(dest, dest + 3); }, std::runtime_error);
    EXPECT_EQ(ThrowOnThirdConstruct::construct_count, 0);
}
