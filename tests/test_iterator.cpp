#include "iterator.hpp"

#include <gtest/gtest.h>

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

TEST(Iterator, InputOrOutputIteratorConcept) {
    struct Valid {
        Valid &operator++() { return *this; }
        int operator++(int) { return 0; }
        Valid &operator*() { return *this; }
    };
    EXPECT_TRUE(mystd::input_or_output_iterator<Valid>);

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

TEST(Iterator, InputIteratorConcept) {
    struct bad_tag {};

    struct Valid {
        using value_type = int;
        using iterator_category = mystd::forward_iterator_tag;

        Valid &operator++() { return *this; }
        value_type operator++(int) { return 0; }
        value_type &operator*() const {
            static int x;
            return x;
        }
    };
    EXPECT_TRUE(mystd::input_iterator<Valid>);

    struct ChildIteratorCategory {
        using value_type = int;
        using iterator_category = mystd::random_access_iterator_tag;

        ChildIteratorCategory &operator++() { return *this; }
        value_type operator++(int) { return 0; }
        value_type &operator*() const {
            static int x;
            return x;
        }
    };
    EXPECT_TRUE(mystd::input_iterator<ChildIteratorCategory>);

    struct IncorrectIteratorCategory {
        using value_type = int;
        using iterator_category = bad_tag;

        IncorrectIteratorCategory &operator++() { return *this; }
        value_type operator++(int) { return 0; }
        value_type &operator*() const {
            static int x;
            return x;
        }
    };
    EXPECT_TRUE(std::indirectly_readable<IncorrectIteratorCategory>);
    EXPECT_FALSE(mystd::input_iterator<IncorrectIteratorCategory>);

    struct InconsistentValueType {
        using value_type = int;
        using iterator_category = bad_tag;

        InconsistentValueType &operator++() { return *this; }
        value_type operator++(int) { return 0; }

        bool &operator*() const {
            static bool x;
            return x;
        }
    };
    EXPECT_TRUE(std::indirectly_readable<IncorrectIteratorCategory>);
    EXPECT_FALSE(mystd::input_iterator<InconsistentValueType>);
}

TEST(Iterator, OutputIteratorConcept) {
    static int shared;

    struct Valid {
        Valid &operator++() { return *this; }
        int *operator++(int) { return &shared; }
        int &operator*() { return shared; }
    };
    EXPECT_TRUE((mystd::output_iterator<Valid, int>));

    struct NotIndirectlyWriteable {
        NotIndirectlyWriteable &operator++() { return *this; }
        int *operator++(int) { return &shared; }
        const int &operator*() { return shared; }
    };
    EXPECT_TRUE((mystd::input_or_output_iterator<NotIndirectlyWriteable>));
    EXPECT_FALSE((std::indirectly_writable<NotIndirectlyWriteable, int>));

    struct NotIncrementDereferenceableAssignable {
        NotIncrementDereferenceableAssignable &operator++() { return *this; }
        int operator++(int) { return shared; }
        int &operator*() { return shared; }
    };
    EXPECT_TRUE((std::indirectly_writable<NotIncrementDereferenceableAssignable, int>));
    EXPECT_FALSE((mystd::output_iterator<NotIncrementDereferenceableAssignable, int>));
}
