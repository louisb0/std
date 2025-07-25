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

TEST(Hashtable, CommonInsert) {
    unique_table ut;
    std::pair<const char *, int> kv{"a", 1};

    ut.insert(mystd::move(kv));
    ut.insert({"b", 1});
    ut.insert({{"c", 1}, {"d", 1}});
    EXPECT_EQ(ut.size(), 4);

    std::unordered_set<const char *> expected_keys{"a", "b", "c", "d"};
    for (const auto &[k, v] : ut) {
        expected_keys.erase(k);
    }
    EXPECT_EQ(expected_keys.size(), 0);

    // Redundant operations
    ut.insert({"b", 1000});
    ut.insert({{"c", 1000}, {"d", 1000}});
    EXPECT_EQ(ut.size(), 4);

    size_t sum{};
    for (const auto &[_, v] : ut) {
        sum += v;
    }
    EXPECT_EQ(sum, 4);
}

TEST(Hashtable, CommonEraseRangeAndPos) {
    unique_table ut(3);
    ut.max_load_factor(1000);
    ut.insert({{"a", 1}, {"b", 1}, {"c", 1}, {"d", 1}, {"e", 1}});

    auto start_it = mystd::next(ut.begin(), 1);
    auto end_it = mystd::next(ut.begin(), 4);
    std::array<const char *, 3> to_remove{start_it->first, mystd::next(start_it)->first};

    auto it = ut.erase(start_it, end_it);
    EXPECT_EQ(it, end_it);
    EXPECT_EQ(ut.size(), 2);
    for (const auto &v : to_remove) {
        EXPECT_FALSE(ut.contains(v));
    }
}

TEST(Hashtable, UniqueEraseKey) {
    unique_table ut(2);
    ut.max_load_factor(1000);
    ut.insert({{"a", 1}, {"b", 1}, {"c", 1}});

    EXPECT_EQ(ut.erase("a"), 1);
    EXPECT_FALSE(ut.contains("a"));
    EXPECT_EQ(ut.size(), 2);
}

TEST(Hashtable, MultiEraseKey) {
    multi_table mt(2);
    mt.max_load_factor(1000);
    mt.insert({{"a", 1}, {"b", 1}, {"c", 1}, {"b", 2}});

    EXPECT_EQ(mt.erase("b"), 2);
    EXPECT_FALSE(mt.contains("b"));
    EXPECT_EQ(mt.size(), 2);
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
