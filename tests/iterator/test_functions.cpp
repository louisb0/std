#include "bits/iterator_functions.hpp"

#include <gtest/gtest.h>

constexpr size_t count = 3;
constexpr int data[] = {1, 2, 3};

class InputIterator : public mystd::iterator<mystd::input_iterator_tag, int> {
    size_t pos_;

public:
    explicit InputIterator(size_t pos = 0) : pos_(pos) {}

    InputIterator &operator++() {
        ++pos_;
        return *this;
    }
    InputIterator operator++(int) { return InputIterator(pos_++); }

    value_type operator*() const { return data[pos_]; }

    bool operator==(const InputIterator &other) const { return pos_ == other.pos_; }
};

class BidirectionalIterator : public mystd::iterator<mystd::bidirectional_iterator_tag, int> {
    size_t pos_;

public:
    explicit BidirectionalIterator(size_t pos = 0) : pos_(pos) {}

    BidirectionalIterator &operator++() {
        ++pos_;
        return *this;
    }
    BidirectionalIterator operator++(int) { return BidirectionalIterator(pos_++); }

    BidirectionalIterator &operator--() {
        --pos_;
        return *this;
    }
    BidirectionalIterator operator--(int) { return BidirectionalIterator(pos_--); }

    value_type operator*() const { return data[pos_]; }

    bool operator==(const BidirectionalIterator &other) const { return pos_ == other.pos_; }
};

TEST(IteratorFunctions, DistanceRandomAccessIterator) {
    EXPECT_EQ(mystd::distance(data, data + count), count);
}

TEST(IteratorFunctions, DistanceInputIterator) {
    EXPECT_TRUE(mystd::input_iterator<InputIterator>);

    InputIterator begin(0);
    InputIterator end(count);

    EXPECT_EQ(mystd::distance(begin, end), count);
    EXPECT_EQ(mystd::distance(begin, begin), 0);
}

TEST(IteratorFunctions, AdvanceInputOnlyIterator) {
    EXPECT_TRUE(mystd::input_iterator<InputIterator>);

    InputIterator it(0);

    mystd::advance(it, 0);
    EXPECT_EQ(*it, 1);

    mystd::advance(it, 2);
    EXPECT_EQ(*it, 3);

    InputIterator it2(0);
    mystd::advance(it2, 0);
    EXPECT_EQ(*it2, 1);
}

TEST(IteratorFunctions, AdvanceBidirectional) {
    EXPECT_TRUE(mystd::bidirectional_iterator<BidirectionalIterator>);

    BidirectionalIterator it(0);

    mystd::advance(it, 0);
    EXPECT_EQ(*it, 1);

    mystd::advance(it, 2);
    EXPECT_EQ(*it, 3);

    mystd::advance(it, -2);
    EXPECT_EQ(*it, 1);
}

TEST(IteratorFunctions, AdvanceRandomAccessIterator) {
    auto it = data;
    EXPECT_TRUE(mystd::random_access_iterator<decltype(it)>);

    mystd::advance(it, 0);
    EXPECT_EQ(*it, 1);

    mystd::advance(it, 2);
    EXPECT_EQ(*it, 3);

    mystd::advance(it, -2);
    EXPECT_EQ(*it, 1);
}
