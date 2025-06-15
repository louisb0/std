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

    struct DereferenceReturnsValue {
        DereferenceReturnsValue &operator++() { return *this; }
        int operator++(int) { return 0; }

        DereferenceReturnsValue operator*() { return *this; }
    };
    EXPECT_FALSE(mystd::input_or_output_iterator<DereferenceReturnsValue>);
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
