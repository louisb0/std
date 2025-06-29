#include "memory.hpp"

#include <gtest/gtest.h>

#include <string>

TEST(Memory, UninitializedCopyString) {
    constexpr size_t count = 3;
    alignas(std::string) std::byte buffer[sizeof(std::string) * count];
    std::string src[count] = {"hello", "world", "test"};
    std::string *dest = reinterpret_cast<std::string *>(buffer);

    auto result = mystd::uninitialized_copy(src, src + count, dest);

    EXPECT_EQ(result, dest + count);
    for (size_t i = 0; i < count; ++i) {
        EXPECT_EQ(dest[i], src[i]);
    }
}

TEST(Memory, UninitializedDefaultConstruct) {
    constexpr size_t count = 3;
    alignas(std::string) std::byte buffer[sizeof(std::string) * count];
    std::string *dest = reinterpret_cast<std::string *>(buffer);

    mystd::uninitialized_default_construct(dest, dest + count);

    for (size_t i = 0; i < count; ++i) {
        EXPECT_EQ(dest[i], "");
    }
}

TEST(Memory, UninitializedFill) {
    constexpr size_t count = 3;
    alignas(std::string) std::byte buffer[sizeof(std::string) * count];
    std::string *dest = reinterpret_cast<std::string *>(buffer);

    mystd::uninitialized_fill(dest, dest + count, "test");

    for (size_t i = 0; i < count; ++i) {
        EXPECT_EQ(dest[i], "test");
    }
}

class CopyCounter {
public:
    CopyCounter() = default;

    CopyCounter(const CopyCounter &) {
        if (copy_count + 1 == failure_point) {
            throw std::runtime_error("Copy failed at designated point");
        }

        copy_count++;
    }

    ~CopyCounter() {
        if (copy_count > 0) {
            --copy_count;
        }
    }

    static void reset(size_t fail_at = SIZE_MAX) {
        copy_count = 0;
        failure_point = fail_at;
    }

    static size_t get_count() { return copy_count; }

private:
    static size_t copy_count;
    static size_t failure_point;
};

size_t CopyCounter::copy_count = 0;
size_t CopyCounter::failure_point = SIZE_MAX;

TEST(Memory, UninitializedCopyExceptionSafety) {
    constexpr size_t count = 5;
    alignas(CopyCounter) std::byte buffer[sizeof(CopyCounter) * count];
    CopyCounter src[count];
    CopyCounter *dest = reinterpret_cast<CopyCounter *>(buffer);

    CopyCounter::reset(3);

    EXPECT_THROW({ mystd::uninitialized_copy(src, src + count, dest); }, std::runtime_error);
    EXPECT_EQ(CopyCounter::get_count(), 0);
}

class ConstructCounter {
public:
    explicit ConstructCounter(int value = 0) : _value(value) {
        if (construct_count + 1 == failure_point) {
            throw std::runtime_error("Construction failed at designated point");
        }

        construct_count++;
    }

    ~ConstructCounter() {
        if (construct_count > 0) {
            --construct_count;
        }
    }

    int value() const { return _value; }

    static void reset(size_t fail_at = SIZE_MAX) {
        construct_count = 0;
        failure_point = fail_at;
    }

    static size_t get_count() { return construct_count; }

private:
    int _value;
    static size_t construct_count;
    static size_t failure_point;
};

size_t ConstructCounter::construct_count = 0;
size_t ConstructCounter::failure_point = SIZE_MAX;

TEST(Memory, UninitializedDefaultConstructExceptionSafety) {
    constexpr size_t count = 5;
    alignas(ConstructCounter) std::byte buffer[sizeof(ConstructCounter) * count];
    ConstructCounter *dest = reinterpret_cast<ConstructCounter *>(buffer);

    ConstructCounter::reset(3);

    EXPECT_THROW(
        { mystd::uninitialized_default_construct(dest, dest + count); }, std::runtime_error);
    EXPECT_EQ(ConstructCounter::get_count(), 0);
}

TEST(Memory, UninitializedFillExceptionSafety) {
    constexpr size_t count = 5;
    alignas(ConstructCounter) std::byte buffer[sizeof(ConstructCounter) * count];
    ConstructCounter *dest = reinterpret_cast<ConstructCounter *>(buffer);

    ConstructCounter::reset(3);
    EXPECT_THROW({ mystd::uninitialized_fill(dest, dest + count, 1); }, std::runtime_error);
    EXPECT_EQ(ConstructCounter::get_count(), 0);
}
