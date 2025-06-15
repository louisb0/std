#include "iterator.hpp"

#include <gtest/gtest.h>
#include <iterator>

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
        using iterator_category = mystd::output_iterator_tag;

        IncorrectIteratorCategory &operator++() { return *this; }
        value_type operator++(int) { return 0; }
        value_type &operator*() const {
            static int x;
            return x;
        }
    };
    EXPECT_FALSE(mystd::input_iterator<IncorrectIteratorCategory>);

    struct InconsistentValueType {
        using value_type = int;
        using iterator_category = mystd::output_iterator_tag;

        InconsistentValueType &operator++() { return *this; }
        value_type operator++(int) { return 0; }

        bool &operator*() const {
            static bool x;
            return x;
        }
    };
    EXPECT_FALSE(mystd::input_iterator<InconsistentValueType>);
}
