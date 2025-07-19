#include "bits/hashtable_node.hpp"
#include "bits/iterator_concepts.hpp"

#include <gtest/gtest.h>

TEST(Node, IteratorConcept) {
    EXPECT_TRUE((mystd::forward_iterator<mystd::detail::node_iterator<int, false>>));
    EXPECT_TRUE((mystd::forward_iterator<mystd::detail::node_iterator<int, true>>));
}

TEST(Node, IteratorConstruction) {
    mystd::detail::node<int> n{.data = 5};

    mystd::detail::node_iterator<int> it(&n);
    mystd::detail::node_iterator<int, true> cit(it);

    EXPECT_EQ(*it, *cit);
}

TEST(Node, IteratorTraversal) {
    struct Wrapper {
        int v{};
    };
    mystd::detail::node<Wrapper> n2{.data = Wrapper{.v = 2}};
    mystd::detail::node<Wrapper> n1{.next = &n2, .data = Wrapper{.v = 1}};

    auto it = mystd::detail::node_iterator(&n1);

    EXPECT_EQ((*it).v, 1);
    EXPECT_EQ(it->v, 1);
    it->v = 100;

    auto old_it = it++;
    EXPECT_EQ(old_it->v, 100);
    EXPECT_EQ(it->v, 2);

    auto end = mystd::detail::node_iterator<Wrapper>();
    EXPECT_EQ(++it, end);
}

TEST(Node, LocalIteratorConcept) {
    EXPECT_TRUE((mystd::forward_iterator<mystd::detail::local_node_iterator<int, false>>));
    EXPECT_TRUE((mystd::forward_iterator<mystd::detail::local_node_iterator<int, true>>));
}

TEST(Node, LocalIteratorConstruction) {
    mystd::detail::node<int> n{.data = 5};

    mystd::detail::local_node_iterator<int> it(&n, 1, 1);
    mystd::detail::local_node_iterator<int, true> cit(it);

    EXPECT_EQ(*it, *cit);
}

TEST(Node, LocalIteratorTraversal) {
    struct Wrapper {
        int v{};
    };
    mystd::detail::node<Wrapper> n3{.hash = 2, .data = Wrapper{.v = 2}};
    mystd::detail::node<Wrapper> n2{.next = &n3, .hash = 1, .data = Wrapper{.v = 2}};
    mystd::detail::node<Wrapper> n1{.next = &n2, .hash = 1, .data = Wrapper{.v = 1}};

    auto it = mystd::detail::local_node_iterator(&n1, 1, 2);

    EXPECT_EQ((*it).v, 1);
    EXPECT_EQ(it->v, 1);
    it->v = 100;

    auto old_it = it++;
    EXPECT_EQ(old_it->v, 100);
    EXPECT_EQ(it->v, 2);

    auto end = mystd::detail::local_node_iterator<Wrapper>();
    EXPECT_EQ(++it, end);
}
