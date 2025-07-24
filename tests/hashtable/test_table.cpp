#include "bits/hashtable.hpp"

#include <gtest/gtest.h>
#include <unordered_set>
#include <utility>

using unique_table =
    mystd::detail::hashtable<std::pair<const char *, int>, mystd::detail::key_extractor_first,
                             std::hash<const char *>, true>;

using multi_table =
    mystd::detail::hashtable<std::pair<const char *, int>, mystd::detail::key_extractor_first,
                             std::hash<const char *>, false>;

struct FirstBucketHash {
    size_t operator()(const char *) const noexcept { return 0; }
};
using colliding_multi_table =
    mystd::detail::hashtable<std::pair<const char *, int>, mystd::detail::key_extractor_first,
                             FirstBucketHash, false>;

TEST(Hashtable, UniqueEmplace) {
    unique_table ut;

    auto [new_it, new_inserted] = ut.emplace("a", 1);
    EXPECT_EQ(new_it->first, "a");
    EXPECT_EQ(new_it->second, 1);
    EXPECT_TRUE(new_inserted);
    EXPECT_EQ(ut.size(), 1);

    auto [existing_it, duplicate_inserted] = ut.emplace("a", 2);
    EXPECT_EQ(existing_it, new_it);
    EXPECT_EQ(existing_it->second, 1);
    EXPECT_FALSE(duplicate_inserted);
    EXPECT_EQ(ut.size(), 1);
}

TEST(Hashtable, MultiEmplace) {
    colliding_multi_table cmt;

    auto first_it = cmt.emplace("a", 1);
    EXPECT_EQ(first_it->first, "a");
    EXPECT_EQ(first_it->second, 1);
    EXPECT_EQ(cmt.size(), 1);

    auto second_it = cmt.emplace("b", 2);
    EXPECT_EQ(second_it->first, "b");
    EXPECT_EQ(second_it->second, 2);
    EXPECT_EQ(cmt.size(), 2);

    auto third_it = cmt.emplace("a", 2);
    EXPECT_NE(third_it, first_it);
    EXPECT_EQ(third_it->first, "a");
    EXPECT_EQ(third_it->second, 2);
    EXPECT_EQ(cmt.size(), 3);

    // Contiguous ordering, i.e. aab or baa for insertion order aba.
    std::unordered_set<const char *> seen_and_finished;
    for (auto it = cmt.begin(); it != cmt.end(); ++it) {
        auto key = it->first;
        EXPECT_EQ(seen_and_finished.count(key), 0);

        auto next_it = mystd::next(it);
        bool continues = (next_it != cmt.end()) && (next_it->first == key);
        if (!continues) {
            seen_and_finished.emplace(key);
        }
    }
}

TEST(Hashtable, CommonFind) {
    unique_table ut;
    ut.emplace("a", 1);

    auto it = ut.find("a");
    EXPECT_EQ(it->first, "a");
    EXPECT_EQ(it->second, 1);

    EXPECT_EQ(ut.find("NA"), ut.end());
}

TEST(Hashtable, CommonContains) {
    unique_table ut;
    ut.emplace("a", 1);

    EXPECT_TRUE(ut.contains("a"));
    EXPECT_FALSE(ut.contains("b"));
}

TEST(Hashtable, UniqueEqualRange) {
    unique_table ut;
    ut.emplace("a", 1);

    auto [first, last] = ut.equal_range("a");
    EXPECT_EQ(first, ut.find("a"));
    EXPECT_EQ(last, ut.end());
}

TEST(Hashtable, MultiEqualRange) {
    colliding_multi_table cmt;
    cmt.emplace("a", 1);
    cmt.emplace("b", 2);
    cmt.emplace("b", 3);
    cmt.emplace("b", 4);
    cmt.emplace("c", 5);

    auto [first, last] = cmt.equal_range("b");
    EXPECT_EQ(mystd::distance(first, last), 3);

    for (auto it = first; it != last; ++it) {
        EXPECT_EQ(it->first, "b");
    }
}

TEST(Hashtable, UniqueCount) {
    unique_table ut;
    ut.emplace("a", 1);

    EXPECT_EQ(ut.count("a"), 1);
    EXPECT_EQ(ut.count("b"), 0);
}

TEST(Hashtable, MultiCount) {
    multi_table mt;
    mt.emplace("a", 1);
    mt.emplace("a", 1);

    EXPECT_EQ(mt.count("a"), 2);
    EXPECT_EQ(mt.count("b"), 0);
}

TEST(Hashtable, CommonBuckets) {
    colliding_multi_table cmt;
    cmt.emplace("a", 1);
    cmt.emplace("a", 2);
    cmt.emplace("a", 3);

    colliding_multi_table::size_type bucket = cmt.bucket("a");
    EXPECT_EQ(cmt.bucket_size(bucket), 3);

    size_t sum{};
    for (const auto &[_, v] : cmt) {
        sum += v;
    }
    EXPECT_EQ(sum, 1 + 2 + 3);
}

// NOTE: For an explanation on why there is no Unique-Multi special case, see the note in
// detail::hashtable::rehash().
TEST(Hashtable, CommonRehash) {
    unique_table ut(2);
    EXPECT_EQ(ut.bucket_count(), 2);

    ut.emplace("a", 1);
    ut.emplace("b", 2);
    ut.emplace("c", 3);
    EXPECT_NE(ut.bucket_count(), 2);

    std::unordered_set<const char *> seen;
    size_t sum{};
    for (const auto &[k, v] : ut) {
        seen.insert(k);
        sum += v;
    }
    EXPECT_EQ(seen.size(), 3);
    EXPECT_EQ(sum, 1 + 2 + 3);
}
