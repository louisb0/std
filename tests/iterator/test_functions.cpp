#include "iterator.hpp"

#include <gtest/gtest.h>

constexpr size_t count = 3;
constexpr int data[] = {1, 2, 3};

TEST(IteratorFunctions, DistanceRandomAccess) {
    EXPECT_EQ(mystd::distance(data, data + count), count);
}

TEST(IteratorFunctions, DistanceIncrementOnly) {
    class IncrementOnly : public mystd::iterator<mystd::input_iterator_tag, int> {
        size_t pos_;

    public:
        explicit IncrementOnly(size_t pos = 0) : pos_(pos) {}

        IncrementOnly &operator++() {
            ++pos_;
            return *this;
        }
        IncrementOnly operator++(int) { return IncrementOnly(pos_++); }

        value_type operator*() const { return data[pos_]; }

        bool operator==(const IncrementOnly &other) const { return pos_ == other.pos_; }
    };

    static_assert(mystd::input_iterator<IncrementOnly>);

    IncrementOnly begin(0);
    IncrementOnly end(count);

    EXPECT_EQ(mystd::distance(begin, end), count);
    EXPECT_EQ(mystd::distance(begin, begin), 0);
}
