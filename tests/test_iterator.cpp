#include "iterator.hpp"

#include <gtest/gtest.h>

TEST(Iterator, CanReference) {
    // object types - (possibly cv-qualified) non function, non reference, non void
    EXPECT_TRUE(mystd::can_reference<int>);
    EXPECT_TRUE(mystd::can_reference<const int>);
    EXPECT_TRUE(mystd::can_reference<volatile int>);
    EXPECT_TRUE(mystd::can_reference<int *>);
    EXPECT_TRUE(mystd::can_reference<int[5]>);
    EXPECT_TRUE(mystd::can_reference<int (*)()>);
    EXPECT_FALSE(mystd::can_reference<void>);
    EXPECT_FALSE(mystd::can_reference<const void>);
    EXPECT_FALSE(mystd::can_reference<volatile void>);

    // function types without cv and ref
    EXPECT_TRUE(mystd::can_reference<int()>);
    EXPECT_FALSE(mystd::can_reference<int() const>);
    EXPECT_FALSE(mystd::can_reference<int() volatile>);
    EXPECT_FALSE(mystd::can_reference<int() const volatile>);
    EXPECT_FALSE(mystd::can_reference<int() &>);
    EXPECT_FALSE(mystd::can_reference<int() &&>);

    // reference types
    EXPECT_TRUE(mystd::can_reference<int &>);
    EXPECT_TRUE(mystd::can_reference<int (&)()>);
    EXPECT_TRUE(mystd::can_reference<const int &>);
    EXPECT_TRUE(mystd::can_reference<int &&>);
}

TEST(Iterator, MatchesIteratorTag) {
    struct TaggedForwardIterator {
        using iterator_category = mystd::forward_iterator_tag;
    };
    EXPECT_TRUE((mystd::matches_iterator_tag<TaggedForwardIterator, mystd::input_iterator_tag>));
    EXPECT_TRUE((mystd::matches_iterator_tag<TaggedForwardIterator, mystd::forward_iterator_tag>));
    EXPECT_FALSE(
        (mystd::matches_iterator_tag<TaggedForwardIterator, mystd::bidirectional_iterator_tag>));

    struct bad_tag {};
    struct TaggedIncorrectly {
        using iterator_category = bad_tag;
    };
    EXPECT_FALSE((mystd::matches_iterator_tag<TaggedIncorrectly, mystd::input_iterator_tag>));

    struct MissingTag {};
    EXPECT_FALSE((mystd::matches_iterator_tag<MissingTag, mystd::input_iterator_tag>));
}

TEST(Iterator, WeaklyIncrementableConcept) {
    struct Valid {
        Valid &operator++() { return *this; }
        int operator++(int) { return 0; }
    };
    EXPECT_TRUE(mystd::weakly_incrementable<Valid>);

    struct MissingPostIncrement {
        MissingPostIncrement &operator++() { return *this; }
        // int operator++(int) { return 0; }
    };
    EXPECT_FALSE(mystd::weakly_incrementable<MissingPostIncrement>);

    struct MissingPreIncrement {
        // MissingPreIncrement &operator++() { return *this; }
        int operator++(int) { return 0; }
    };
    EXPECT_FALSE(mystd::weakly_incrementable<MissingPreIncrement>);

    struct PreIncrementReturnsValue {
        PreIncrementReturnsValue operator++() { return *this; }
        int operator++(int) { return 0; }
    };
    EXPECT_FALSE(mystd::weakly_incrementable<PreIncrementReturnsValue>);
}

TEST(Iterator, IncrementableConcept) {
    struct Valid {
        Valid &operator++() { return *this; }
        Valid operator++(int) { return *this; }

        bool operator==(const Valid &other) const { return true; }
    };
    EXPECT_TRUE(mystd::incrementable<Valid>);

    struct NotWeaklyIncrementable {
        // NotWeaklyIncrementable &operator++() { return *this; }
        NotWeaklyIncrementable operator++(int) { return *this; }

        bool operator==(const Valid &other) const { return true; }
    };
    EXPECT_FALSE(mystd::weakly_incrementable<NotWeaklyIncrementable>);
    EXPECT_FALSE(mystd::incrementable<NotWeaklyIncrementable>);

    struct NotRegular {
        NotRegular &operator++() { return *this; }
        NotRegular operator++(int) { return *this; }

        // bool operator==(const NotRegular &other) const { return true; }
    };
    EXPECT_FALSE(std::regular<NotRegular>);
    EXPECT_FALSE(mystd::incrementable<NotRegular>);

    struct PostIncrementNotCopy {
        PostIncrementNotCopy &operator++() { return *this; }
        PostIncrementNotCopy &operator++(int) { return *this; }

        bool operator==(const PostIncrementNotCopy &other) const { return true; }
    };
    EXPECT_FALSE(mystd::incrementable<PostIncrementNotCopy>);
}

TEST(Iterator, InputOrOutputIteratorConcept) {
    struct Valid {
        Valid &operator++() { return *this; }
        int operator++(int) { return 0; }

        Valid &operator*() { return *this; }
    };
    EXPECT_TRUE(mystd::input_or_output_iterator<Valid>);

    struct NotWeaklyIncrementable {
        // NotWeaklyIncrementable &operator++() { return *this; }
        // int operator++(int) { return 0; }

        NotWeaklyIncrementable &operator*() { return *this; }
    };
    EXPECT_FALSE(mystd::weakly_incrementable<NotWeaklyIncrementable>);
    EXPECT_FALSE(mystd::input_or_output_iterator<NotWeaklyIncrementable>);

    struct MissingDereferenceOperator {
        MissingDereferenceOperator &operator++() { return *this; }
        int operator++(int) { return 0; }

        // MissingDereferenceOperator &operator*() { return *this; }
    };
    EXPECT_FALSE(mystd::input_or_output_iterator<MissingDereferenceOperator>);

    struct DereferenceNotReferencable {
        DereferenceNotReferencable &operator++() { return *this; }
        int operator++(int) { return 0; }

        void operator*() { return; }
    };
    EXPECT_FALSE(mystd::input_or_output_iterator<DereferenceNotReferencable>);
}

TEST(Iterator, OutputIteratorConcept) {
    static int shared;

    struct Valid {
        Valid &operator++() { return *this; }
        int *operator++(int) { return 0; }

        int &operator*() { return shared; }
    };
    EXPECT_TRUE((mystd::output_iterator<Valid, int>));

    struct NotInputOrOutput {
        NotInputOrOutput &operator++() { return *this; }
        int *operator++(int) { return 0; }

        // !can_reference<void> => !input_or_output<I>
        void operator*() { return; }
    };
    EXPECT_FALSE(mystd::input_or_output_iterator<NotInputOrOutput>);
    EXPECT_FALSE((mystd::output_iterator<NotInputOrOutput, int>));

    struct NotIndirectlyWriteable {
        NotIndirectlyWriteable &operator++() { return *this; }
        int *operator++(int) { return &shared; }

        const int &operator*() { return shared; }
    };
    EXPECT_TRUE((mystd::input_or_output_iterator<NotIndirectlyWriteable>));
    EXPECT_FALSE((std::indirectly_writable<NotIndirectlyWriteable, int>));
    EXPECT_FALSE((mystd ::output_iterator<NotIndirectlyWriteable, int>));

    struct NotIncrementDereferenceableAssignable {
        NotIncrementDereferenceableAssignable &operator++() { return *this; }
        // Must be able to dereference and assign to the pre-increment.
        int operator++(int) { return shared; }

        int &operator*() { return shared; }
    };
    EXPECT_TRUE((mystd::input_or_output_iterator<NotIncrementDereferenceableAssignable>));
    EXPECT_TRUE((std::indirectly_writable<NotIncrementDereferenceableAssignable, int>));
    EXPECT_FALSE((mystd::output_iterator<NotIncrementDereferenceableAssignable, int>));
}

TEST(Iterator, InputIteratorConcept) {
    static int shared;

    struct Valid {
        using value_type = int;
        using iterator_category = mystd::input_iterator_tag;

        Valid &operator++() { return *this; }
        value_type *operator++(int) { return 0; }

        value_type &operator*() const { return shared; }
        bool operator==(const Valid &other) const { return true; }
    };
    EXPECT_TRUE(mystd::input_iterator<Valid>);

    struct NotInputOrOutputIterator {
        using value_type = int;
        using iterator_category = mystd::input_iterator_tag;

        // Must be able to assign to the dereferenced post-increment.
        NotInputOrOutputIterator operator++() { return *this; }
        value_type *operator++(int) { return 0; }

        value_type &operator*() const { return shared; }
        bool operator==(const Valid &other) const { return true; }
    };
    EXPECT_FALSE(mystd::input_or_output_iterator<NotInputOrOutputIterator>);
    EXPECT_TRUE(std::indirectly_readable<NotInputOrOutputIterator>);
    EXPECT_FALSE(mystd::input_iterator<NotInputOrOutputIterator>);

    struct NotIndirectlyReadable {
        using value_type = int;
        using iterator_category = mystd::input_iterator_tag;

        NotIndirectlyReadable &operator++() { return *this; }
        value_type *operator++(int) { return 0; }

        // non-const => !indirectly_readable<I>
        value_type &operator*() { return shared; }
        bool operator==(const Valid &other) const { return true; }
    };
    EXPECT_TRUE(mystd::input_or_output_iterator<NotIndirectlyReadable>);
    EXPECT_FALSE(std::indirectly_readable<NotIndirectlyReadable>);
    EXPECT_FALSE(mystd::input_iterator<NotIndirectlyReadable>);

    struct NotEqualityComparable {
        using value_type = int;
        using iterator_category = mystd::input_iterator_tag;

        NotEqualityComparable &operator++() { return *this; }
        value_type *operator++(int) { return 0; }

        value_type &operator*() const { return shared; }
        // bool operator==(const NotEqualityComparable &other) const { return true; }
    };
    EXPECT_TRUE(mystd::input_or_output_iterator<NotEqualityComparable>);
    EXPECT_TRUE(std::indirectly_readable<NotEqualityComparable>);
    EXPECT_FALSE(mystd::input_iterator<NotEqualityComparable>);
}

TEST(Iterator, ForwardIteratorConcept) {
    static int shared;

    struct Valid {
        using value_type = int;
        using iterator_category = mystd::forward_iterator_tag;

        Valid &operator++() { return *this; }
        Valid operator++(int) { return *this; }

        value_type &operator*() const { return shared; }
        bool operator==(const Valid &other) const { return true; }
    };
    EXPECT_TRUE(mystd::forward_iterator<Valid>);

    struct NotInputIterator {
        using value_type = int;
        using iterator_category = mystd::forward_iterator_tag;

        NotInputIterator &operator++() { return *this; }
        NotInputIterator operator++(int) { return *this; }

        // non-const => !indirectly_readable<I> => !input_iterator<I>
        value_type &operator*() { return shared; }
        bool operator==(const NotInputIterator &other) const { return true; }
    };
    EXPECT_FALSE(mystd::input_iterator<NotInputIterator>);
    EXPECT_TRUE(mystd::incrementable<NotInputIterator>);
    EXPECT_FALSE(mystd::forward_iterator<NotInputIterator>);

    struct NotIncrementable {
        using value_type = int;
        using iterator_category = mystd::forward_iterator_tag;

        NotIncrementable &operator++() { return *this; }
        // Pre-increment must return a copy of I.
        NotIncrementable &operator++(int) { return *this; }

        value_type &operator*() const { return shared; }
        bool operator==(const NotIncrementable &other) const { return true; }
    };
    EXPECT_TRUE(mystd::input_iterator<NotIncrementable>);
    EXPECT_FALSE(mystd::incrementable<NotIncrementable>);
    EXPECT_FALSE(mystd::forward_iterator<NotIncrementable>);
}

TEST(Iterator, BidirectionalIteratorConcept) {
    static int shared;

    struct Valid {
        using value_type = int;
        using iterator_category = mystd::bidirectional_iterator_tag;

        Valid &operator++() { return *this; }
        Valid operator++(int) { return *this; }

        value_type &operator*() const { return shared; }
        bool operator==(const Valid &other) const { return true; }

        Valid &operator--() { return *this; }
        Valid operator--(int) { return *this; }
    };
    EXPECT_TRUE(mystd::bidirectional_iterator<Valid>);

    struct NotForwardIterator {
        using value_type = int;
        using iterator_category = mystd::bidirectional_iterator_tag;

        // NotForwardIterator &operator++() { return *this; }
        // NotForwardIterator operator++(int) { return *this; }

        value_type &operator*() const { return shared; }
        bool operator==(const NotForwardIterator &other) const { return true; }

        NotForwardIterator &operator--() { return *this; }
        NotForwardIterator operator--(int) { return *this; }
    };
    EXPECT_FALSE(mystd::forward_iterator<NotForwardIterator>);
    EXPECT_FALSE(mystd::bidirectional_iterator<NotForwardIterator>);

    struct NotDecrementable {
        using value_type = int;
        using iterator_category = mystd::bidirectional_iterator_tag;

        NotDecrementable &operator++() { return *this; }
        NotDecrementable operator++(int) { return *this; }

        value_type &operator*() const { return shared; }
        bool operator==(const NotDecrementable &other) const { return true; }

        // NotDecrementable &operator--() { return *this; }
        // NotDecrementable operator--(int) { return *this; }
    };
    EXPECT_TRUE(mystd::forward_iterator<NotDecrementable>);
    EXPECT_FALSE(mystd::bidirectional_iterator<NotDecrementable>);
}

TEST(Iterator, RandomAccessIteratorConcept) {
    static int shared;

    struct Valid {
        using difference_type = std::ptrdiff_t;
        using value_type = int;
        using pointer = int *;
        using reference = int &;
        using iterator_category = mystd::random_access_iterator_tag;

        Valid &operator++() { return *this; }
        Valid operator++(int) { return *this; }

        reference operator*() const { return shared; }
        bool operator==(const Valid &other) const { return true; }

        Valid &operator--() { return *this; }
        Valid operator--(int) { return *this; }

        bool operator<(const Valid &other) const { return true; }
        bool operator<=(const Valid &other) const { return true; }
        bool operator>(const Valid &other) const { return false; }
        bool operator>=(const Valid &other) const { return false; }

        difference_type operator-(const Valid &other) const { return 1; }

        Valid &operator+=(difference_type n) { return *this; }
        Valid &operator-=(difference_type n) { return *this; }

        Valid operator+(difference_type n) const { return *this; }
        Valid operator-(difference_type n) const { return *this; }

        reference operator[](difference_type n) const { return shared; }
    };
    EXPECT_TRUE(mystd::random_access_iterator<Valid>);

    struct NotBidirectional {
        using difference_type = std::ptrdiff_t;
        using value_type = int;
        using pointer = int *;
        using reference = int &;
        using iterator_category = mystd::random_access_iterator_tag;

        NotBidirectional &operator++() { return *this; }
        NotBidirectional operator++(int) { return *this; }

        reference operator*() const { return shared; }
        bool operator==(const NotBidirectional &other) const { return true; }

        // NotBidirectional &operator--() { return *this; }
        // NotBidirectional operator--(int) { return *this; }

        bool operator<(const NotBidirectional &other) const { return true; }
        bool operator<=(const NotBidirectional &other) const { return true; }
        bool operator>(const NotBidirectional &other) const { return false; }
        bool operator>=(const NotBidirectional &other) const { return false; }

        difference_type operator-(const NotBidirectional &other) const { return 1; }

        NotBidirectional &operator+=(difference_type n) { return *this; }
        NotBidirectional &operator-=(difference_type n) { return *this; }

        NotBidirectional operator+(difference_type n) const { return *this; }
        NotBidirectional operator-(difference_type n) const { return *this; }

        reference operator[](difference_type n) const { return shared; }
    };
    EXPECT_FALSE(mystd::bidirectional_iterator<NotBidirectional>);
    EXPECT_FALSE(mystd::random_access_iterator<NotBidirectional>);

    struct NotTotallyOrdered {
        using difference_type = std::ptrdiff_t;
        using value_type = int;
        using pointer = int *;
        using reference = int &;
        using iterator_category = mystd::random_access_iterator_tag;

        NotTotallyOrdered &operator++() { return *this; }
        NotTotallyOrdered operator++(int) { return *this; }

        reference operator*() const { return shared; }
        bool operator==(const NotTotallyOrdered &other) const { return true; }

        NotTotallyOrdered &operator--() { return *this; }
        NotTotallyOrdered operator--(int) { return *this; }

        // bool operator<(const NotTotallyOrdered &other) const { return true; }
        // bool operator<=(const NotTotallyOrdered &other) const { return true; }
        // bool operator>(const NotTotallyOrdered &other) const { return false; }
        // bool operator>=(const NotTotallyOrdered &other) const { return false; }

        difference_type operator-(const NotTotallyOrdered &other) const { return 1; }

        NotTotallyOrdered &operator+=(difference_type n) { return *this; }
        NotTotallyOrdered &operator-=(difference_type n) { return *this; }

        NotTotallyOrdered operator+(difference_type n) const { return *this; }
        NotTotallyOrdered operator-(difference_type n) const { return *this; }

        reference operator[](difference_type n) const { return shared; }
    };
    EXPECT_TRUE(mystd::bidirectional_iterator<NotTotallyOrdered>);
    EXPECT_FALSE(std::totally_ordered<NotTotallyOrdered>);
    EXPECT_FALSE(mystd::random_access_iterator<NotTotallyOrdered>);

    struct NotDifferenceable {
        using difference_type = std::ptrdiff_t;
        using value_type = int;
        using pointer = int *;
        using reference = int &;
        using iterator_category = mystd::random_access_iterator_tag;

        NotDifferenceable &operator++() { return *this; }
        NotDifferenceable operator++(int) { return *this; }

        reference operator*() const { return shared; }
        bool operator==(const NotDifferenceable &other) const { return true; }

        NotDifferenceable &operator--() { return *this; }
        NotDifferenceable operator--(int) { return *this; }

        bool operator<(const NotDifferenceable &other) const { return true; }
        bool operator<=(const NotDifferenceable &other) const { return true; }
        bool operator>(const NotDifferenceable &other) const { return false; }
        bool operator>=(const NotDifferenceable &other) const { return false; }

        // difference_type operator-(const NotDifferenceable &other) const { return 1; }

        NotDifferenceable &operator+=(difference_type n) { return *this; }
        NotDifferenceable &operator-=(difference_type n) { return *this; }

        NotDifferenceable operator+(difference_type n) const { return *this; }
        NotDifferenceable operator-(difference_type n) const { return *this; }

        reference operator[](difference_type n) const { return shared; }
    };
    EXPECT_TRUE(mystd::bidirectional_iterator<NotDifferenceable>);
    EXPECT_TRUE(std::totally_ordered<NotDifferenceable>);
    EXPECT_FALSE((std::sized_sentinel_for<NotDifferenceable, NotDifferenceable>));
    EXPECT_FALSE(mystd::random_access_iterator<NotDifferenceable>);

    struct MissingNumericOverload {
        using difference_type = std::ptrdiff_t;
        using value_type = int;
        using pointer = int *;
        using reference = int &;
        using iterator_category = mystd::random_access_iterator_tag;

        MissingNumericOverload &operator++() { return *this; }
        MissingNumericOverload operator++(int) { return *this; }

        reference operator*() const { return shared; }
        bool operator==(const MissingNumericOverload &other) const { return true; }

        MissingNumericOverload &operator--() { return *this; }
        MissingNumericOverload operator--(int) { return *this; }

        bool operator<(const MissingNumericOverload &other) const { return true; }
        bool operator<=(const MissingNumericOverload &other) const { return true; }
        bool operator>(const MissingNumericOverload &other) const { return false; }
        bool operator>=(const MissingNumericOverload &other) const { return false; }

        difference_type operator-(const MissingNumericOverload &other) const { return 1; }

        // MissingNumericOverload &operator+=(difference_type n) { return *this; }
        // MissingNumericOverload &operator-=(difference_type n) { return *this; }
        //
        // MissingNumericOverload operator+(difference_type n) const { return *this; }
        // MissingNumericOverload operator-(difference_type n) const { return *this; }
        //
        // reference operator[](difference_type n) const { return shared; }
    };
    EXPECT_TRUE(mystd::bidirectional_iterator<MissingNumericOverload>);
    EXPECT_TRUE(std::totally_ordered<MissingNumericOverload>);
    EXPECT_TRUE((std::sized_sentinel_for<MissingNumericOverload, MissingNumericOverload>));
    EXPECT_FALSE(mystd::random_access_iterator<MissingNumericOverload>);
}

TEST(Iterator, ReverseIterator) {
    int data[5] = {1, 2, 3, 4, 5};

    // Constructors
    mystd::reverse_iterator<int *> rit(data + 5);
    EXPECT_EQ(rit.base(), data + 5);

    mystd::reverse_iterator<int *> default_rit;
    EXPECT_EQ(default_rit.base(), (int *){});

    mystd::reverse_iterator<const int *> const_rit(rit);
    EXPECT_EQ(const_rit.base(), rit.base());

    // operator->
    struct Wrapper {
        int v;
    };
    Wrapper structs[3] = {{10}, {20}, {30}};
    mystd::reverse_iterator<Wrapper *> struct_rit(structs + 3);
    EXPECT_EQ(struct_rit->v, 30);

    // operator++
    mystd::reverse_iterator pre_inc(data + 5);
    auto &pre_inc_ref = ++pre_inc;
    EXPECT_EQ(*pre_inc, 4);
    EXPECT_EQ(&pre_inc_ref, &pre_inc);

    // operator--
    mystd::reverse_iterator pre_dec(data + 4);
    auto &pre_dec_ref = --pre_dec;
    EXPECT_EQ(*pre_dec, 5);
    EXPECT_EQ(&pre_dec_ref, &pre_dec);

    // operator++(int)
    mystd::reverse_iterator post_inc(data + 5);
    auto post_inc_old = post_inc++;
    EXPECT_EQ(*post_inc, 4);
    EXPECT_EQ(*post_inc_old, 5);

    // operator--(int)
    mystd::reverse_iterator post_dec(data + 4);
    auto post_dec_old = post_dec--;
    EXPECT_EQ(*post_dec, 5);
    EXPECT_EQ(*post_dec_old, 4);

    // operator+ / operator-
    mystd::reverse_iterator arith_rit(data + 5);

    auto plus_result = arith_rit + 4;
    EXPECT_EQ(*plus_result, 1);
    EXPECT_EQ(*arith_rit, 5);

    auto commutative_result = 4 + arith_rit;
    EXPECT_EQ(*commutative_result, 1);
    EXPECT_EQ(*arith_rit, 5);

    plus_result = plus_result - 4;
    EXPECT_EQ(*plus_result, 5);
    EXPECT_EQ(*arith_rit, 5);

    // operator- non-member difference
    mystd::reverse_iterator diff_left(data + 5);
    mystd::reverse_iterator diff_right(data);
    EXPECT_EQ(diff_left - diff_right, -5);
    diff_left++;
    EXPECT_EQ(diff_right - diff_left, 4);

    // operator+=
    mystd::reverse_iterator plus_eq(data + 5);
    EXPECT_EQ(&(plus_eq += 3), &plus_eq);
    EXPECT_EQ(*plus_eq, 2);

    // operator-=
    mystd::reverse_iterator minus_eq(data + 2);
    EXPECT_EQ(&(minus_eq -= 3), &minus_eq);
    EXPECT_EQ(*minus_eq, 5);

    // comparison
    mystd::reverse_iterator high(data);
    mystd::reverse_iterator mid(data + 3);
    mystd::reverse_iterator low(data + 5);

    EXPECT_LT(low, mid);
    EXPECT_LE(mid, high);

    EXPECT_GT(high, mid);
    EXPECT_GE(mid, low);

    EXPECT_TRUE(low == low);
    EXPECT_FALSE(low == mid);

    EXPECT_FALSE(low != low);
    EXPECT_TRUE(low != mid);
}
}
