#include "iterator.hpp"

#include <concepts>
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
    };
    EXPECT_FALSE(mystd::weakly_incrementable<MissingPostIncrement>);

    struct MissingPreIncrement {
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
        NotWeaklyIncrementable operator++(int) { return *this; }

        bool operator==(const Valid &other) const { return true; }
    };
    EXPECT_FALSE(mystd::weakly_incrementable<NotWeaklyIncrementable>);
    EXPECT_FALSE(mystd::incrementable<NotWeaklyIncrementable>);

    struct NotRegular {
        NotRegular &operator++() { return *this; }
        NotRegular operator++(int) { return *this; }
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
        NotWeaklyIncrementable &operator*() { return *this; }
    };
    EXPECT_FALSE(mystd::weakly_incrementable<NotWeaklyIncrementable>);
    EXPECT_FALSE(mystd::input_or_output_iterator<NotWeaklyIncrementable>);

    struct MissingDereferenceOperator {
        MissingDereferenceOperator &operator++() { return *this; }
        int operator++(int) { return 0; }
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
    };
    EXPECT_TRUE(mystd::input_iterator<Valid>);

    struct NotInputOrOutputIterator {
        using value_type = int;
        using iterator_category = mystd::input_iterator_tag;

        NotInputOrOutputIterator operator++() { return *this; }
        value_type *operator++(int) { return 0; }

        value_type &operator*() const { return shared; }
    };
    EXPECT_FALSE(mystd::input_or_output_iterator<NotInputOrOutputIterator>);
    EXPECT_TRUE(std::indirectly_readable<NotInputOrOutputIterator>);
    EXPECT_FALSE(mystd::input_iterator<NotInputOrOutputIterator>);

    struct NotIndirectlyReadable {
        using value_type = int;
        using iterator_category = mystd::input_iterator_tag;

        NotIndirectlyReadable &operator++() { return *this; }
        value_type *operator++(int) { return 0; }

        value_type &operator*() { return shared; }
    };
    EXPECT_TRUE(mystd::input_or_output_iterator<NotIndirectlyReadable>);
    EXPECT_FALSE(std::indirectly_readable<NotIndirectlyReadable>);
    EXPECT_FALSE(mystd::input_iterator<NotIndirectlyReadable>);

    struct NoTag {
        using value_type = int;

        NoTag &operator++() { return *this; }
        value_type *operator++(int) { return 0; }

        value_type &operator*() const { return shared; }
    };
    EXPECT_FALSE(mystd::input_iterator<NoTag>);
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
        NotIncrementable &operator++(int) { return *this; }

        value_type &operator*() const { return shared; }

        bool operator==(const NotIncrementable &other) const { return true; }
    };
    EXPECT_TRUE(mystd::input_iterator<NotIncrementable>);
    EXPECT_FALSE(mystd::incrementable<NotIncrementable>);
    EXPECT_FALSE(mystd::forward_iterator<NotIncrementable>);

    struct NoTag {
        using value_type = int;

        NoTag &operator++() { return *this; }
        NoTag operator++(int) { return *this; }

        value_type &operator*() const { return shared; }

        bool operator==(const NoTag &other) const { return true; }
    };
    EXPECT_FALSE(mystd::forward_iterator<NoTag>);
}
