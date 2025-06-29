#include "iterator.hpp"

#include <gtest/gtest.h>

TEST(IteratorConcepts, WeaklyIncrementableConcept) {
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

TEST(IteratorConcepts, IncrementableConcept) {
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

TEST(IteratorConcepts, InputOrOutputIteratorConcept) {
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

TEST(IteratorConcepts, OutputIteratorConcept) {
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

TEST(IteratorConcepts, InputIteratorConcept) {
    static int shared;

    struct Valid : mystd::iterator<mystd::input_iterator_tag, int> {
        Valid &operator++() { return *this; }
        value_type *operator++(int) { return 0; }

        value_type &operator*() const { return shared; }
        bool operator==(const Valid &other) const { return true; }
    };
    EXPECT_TRUE(mystd::input_iterator<Valid>);

    struct NotInputOrOutputIterator : mystd::iterator<mystd::input_iterator_tag, int> {
        // Must be able to assign to the dereferenced post-increment.
        NotInputOrOutputIterator operator++() { return *this; }
        value_type *operator++(int) { return 0; }

        value_type &operator*() const { return shared; }
        bool operator==(const Valid &other) const { return true; }
    };
    EXPECT_FALSE(mystd::input_or_output_iterator<NotInputOrOutputIterator>);
    EXPECT_TRUE(std::indirectly_readable<NotInputOrOutputIterator>);
    EXPECT_FALSE(mystd::input_iterator<NotInputOrOutputIterator>);

    struct NotIndirectlyReadable : mystd::iterator<mystd::input_iterator_tag, int> {
        NotIndirectlyReadable &operator++() { return *this; }
        value_type *operator++(int) { return 0; }

        // non-const => !indirectly_readable<I>
        value_type &operator*() { return shared; }
        bool operator==(const Valid &other) const { return true; }
    };
    EXPECT_TRUE(mystd::input_or_output_iterator<NotIndirectlyReadable>);
    EXPECT_FALSE(std::indirectly_readable<NotIndirectlyReadable>);
    EXPECT_FALSE(mystd::input_iterator<NotIndirectlyReadable>);

    struct NotEqualityComparable : mystd::iterator<mystd::input_iterator_tag, int> {
        NotEqualityComparable &operator++() { return *this; }
        value_type *operator++(int) { return 0; }

        value_type &operator*() const { return shared; }
        // bool operator==(const NotEqualityComparable &other) const { return true; }
    };
    EXPECT_TRUE(mystd::input_or_output_iterator<NotEqualityComparable>);
    EXPECT_TRUE(std::indirectly_readable<NotEqualityComparable>);
    EXPECT_FALSE(mystd::input_iterator<NotEqualityComparable>);
}

TEST(IteratorConcepts, ForwardIteratorConcept) {
    static int shared;

    struct Valid : mystd::iterator<mystd::forward_iterator_tag, int> {
        Valid &operator++() { return *this; }
        Valid operator++(int) { return *this; }

        value_type &operator*() const { return shared; }
        bool operator==(const Valid &other) const { return true; }
    };
    EXPECT_TRUE(mystd::forward_iterator<Valid>);

    struct NotInputIterator : mystd::iterator<mystd::forward_iterator_tag, int> {
        NotInputIterator &operator++() { return *this; }
        NotInputIterator operator++(int) { return *this; }

        // non-const => !indirectly_readable<I> => !input_iterator<I>
        value_type &operator*() { return shared; }
        bool operator==(const NotInputIterator &other) const { return true; }
    };
    EXPECT_FALSE(mystd::input_iterator<NotInputIterator>);
    EXPECT_TRUE(mystd::incrementable<NotInputIterator>);
    EXPECT_FALSE(mystd::forward_iterator<NotInputIterator>);

    struct NotIncrementable : mystd::iterator<mystd::forward_iterator_tag, int> {
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

TEST(IteratorConcepts, BidirectionalIteratorConcept) {
    static int shared;

    struct Valid : mystd::iterator<mystd::bidirectional_iterator_tag, int> {
        Valid &operator++() { return *this; }
        Valid operator++(int) { return *this; }

        value_type &operator*() const { return shared; }
        bool operator==(const Valid &other) const { return true; }

        Valid &operator--() { return *this; }
        Valid operator--(int) { return *this; }
    };
    EXPECT_TRUE(mystd::bidirectional_iterator<Valid>);

    struct NotForwardIterator : mystd::iterator<mystd::bidirectional_iterator_tag, int> {
        // NotForwardIterator &operator++() { return *this; }
        // NotForwardIterator operator++(int) { return *this; }

        value_type &operator*() const { return shared; }
        bool operator==(const NotForwardIterator &other) const { return true; }

        NotForwardIterator &operator--() { return *this; }
        NotForwardIterator operator--(int) { return *this; }
    };
    EXPECT_FALSE(mystd::forward_iterator<NotForwardIterator>);
    EXPECT_FALSE(mystd::bidirectional_iterator<NotForwardIterator>);

    struct NotDecrementable : mystd::iterator<mystd::bidirectional_iterator_tag, int> {
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

TEST(IteratorConcepts, RandomAccessIteratorConcept) {
    static int shared;

    struct Valid : mystd::iterator<mystd::random_access_iterator_tag, int> {
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

    struct NotBidirectional : mystd::iterator<mystd::random_access_iterator_tag, int> {
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

    struct NotTotallyOrdered : mystd::iterator<mystd::random_access_iterator_tag, int> {
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

    struct NotDifferenceable : mystd::iterator<mystd::random_access_iterator_tag, int> {
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

    struct MissingNumericOverload : mystd::iterator<mystd::random_access_iterator_tag, int> {
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
