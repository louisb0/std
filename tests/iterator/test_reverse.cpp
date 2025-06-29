#include "iterator.hpp"

#include <gtest/gtest.h>

class ReverseIterator : public ::testing::Test {
protected:
    void SetUp() override {
        data[0] = 1;
        data[1] = 2;
        data[2] = 3;
        data[3] = 4;
        data[4] = 5;

        structs[0] = {10};
        structs[1] = {20};
        structs[2] = {30};
    }

    int data[5];
    struct Wrapper {
        int v;
    };
    Wrapper structs[3];
};

TEST_F(ReverseIterator, DefaultConstructor) {
    mystd::reverse_iterator<int *> default_rit;
    EXPECT_EQ(default_rit.base(), (int *){});
}

TEST_F(ReverseIterator, ExplicitConstructor) {
    mystd::reverse_iterator<int *> rit(data + 5);
    EXPECT_EQ(rit.base(), data + 5);
}

TEST_F(ReverseIterator, CopyConstructor) {
    mystd::reverse_iterator<int *> rit(data + 5);
    mystd::reverse_iterator<const int *> const_rit(rit);
    EXPECT_EQ(const_rit.base(), rit.base());
}

TEST_F(ReverseIterator, DereferenceOperator) {
    mystd::reverse_iterator<int *> rit(data + 5);
    EXPECT_EQ(*rit, 5);

    mystd::reverse_iterator<int *> rit2(data + 3);
    EXPECT_EQ(*rit2, 3);
}

TEST_F(ReverseIterator, ArrowOperator) {
    mystd::reverse_iterator<Wrapper *> struct_rit(structs + 3);
    EXPECT_EQ(struct_rit->v, 30);

    mystd::reverse_iterator<Wrapper *> struct_rit2(structs + 2);
    EXPECT_EQ(struct_rit2->v, 20);
}

TEST_F(ReverseIterator, PreIncrementOperator) {
    mystd::reverse_iterator<int *> pre_inc(data + 5);
    auto &pre_inc_ref = ++pre_inc;
    EXPECT_EQ(*pre_inc, 4);
    EXPECT_EQ(&pre_inc_ref, &pre_inc);
}

TEST_F(ReverseIterator, PostIncrementOperator) {
    mystd::reverse_iterator<int *> post_inc(data + 5);
    auto post_inc_old = post_inc++;
    EXPECT_EQ(*post_inc, 4);
    EXPECT_EQ(*post_inc_old, 5);
}

TEST_F(ReverseIterator, PreDecrementOperator) {
    mystd::reverse_iterator<int *> pre_dec(data + 4);
    auto &pre_dec_ref = --pre_dec;
    EXPECT_EQ(*pre_dec, 5);
    EXPECT_EQ(&pre_dec_ref, &pre_dec);
}

TEST_F(ReverseIterator, PostDecrementOperator) {
    mystd::reverse_iterator<int *> post_dec(data + 4);
    auto post_dec_old = post_dec--;
    EXPECT_EQ(*post_dec, 5);
    EXPECT_EQ(*post_dec_old, 4);
}

TEST_F(ReverseIterator, AdditionOperator) {
    mystd::reverse_iterator<int *> arith_rit(data + 5);

    auto plus_result = arith_rit + 4;
    EXPECT_EQ(*plus_result, 1);
    EXPECT_EQ(*arith_rit, 5); // Original unchanged
}

TEST_F(ReverseIterator, CommutativeAddition) {
    mystd::reverse_iterator<int *> arith_rit(data + 5);

    auto commutative_result = 4 + arith_rit;
    EXPECT_EQ(*commutative_result, 1);
    EXPECT_EQ(*arith_rit, 5);
}

TEST_F(ReverseIterator, SubtractionOperator) {
    mystd::reverse_iterator<int *> arith_rit(data + 5);
    auto plus_result = arith_rit + 4;

    plus_result = plus_result - 4;
    EXPECT_EQ(*plus_result, 5);
}

TEST_F(ReverseIterator, DifferenceOperator) {
    mystd::reverse_iterator<int *> diff_left(data + 5);
    mystd::reverse_iterator<int *> diff_right(data);

    EXPECT_EQ(diff_right - diff_left, 5);
    diff_left++;
    EXPECT_EQ(diff_left - diff_right, -4);
}

TEST_F(ReverseIterator, PlusEqualsOperator) {
    mystd::reverse_iterator<int *> plus_eq(data + 5);
    EXPECT_EQ(&(plus_eq += 3), &plus_eq);
    EXPECT_EQ(*plus_eq, 2);
}

TEST_F(ReverseIterator, MinusEqualsOperator) {
    mystd::reverse_iterator<int *> minus_eq(data + 2);
    EXPECT_EQ(&(minus_eq -= 3), &minus_eq);
    EXPECT_EQ(*minus_eq, 5);
}

TEST_F(ReverseIterator, ComparisonOperators) {
    mystd::reverse_iterator<int *> high(data);
    mystd::reverse_iterator<int *> mid(data + 3);
    mystd::reverse_iterator<int *> low(data + 5);

    EXPECT_LT(low, mid);
    EXPECT_LT(mid, high);

    EXPECT_LE(low, mid);
    EXPECT_LE(mid, high);
    EXPECT_LE(low, low);

    EXPECT_GT(high, mid);
    EXPECT_GT(mid, low);

    EXPECT_GE(high, mid);
    EXPECT_GE(mid, low);
    EXPECT_GE(low, low);

    EXPECT_TRUE(low == low);
    EXPECT_FALSE(low == mid);

    EXPECT_FALSE(low != low);
    EXPECT_TRUE(low != mid);
}
