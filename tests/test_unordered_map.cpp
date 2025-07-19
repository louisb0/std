#include "unordered_map.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

TEST(UnorderedMap, Emplace) {
    mystd::unordered_map<const char *, int> map;

    auto [it, success] = map.emplace("a", 1);
    EXPECT_TRUE(success);
    EXPECT_EQ(it->first, "a");
    EXPECT_EQ(it->second, 1);
    EXPECT_FALSE(map.empty());
    EXPECT_EQ(map.size(), 1);

    auto [it2, success2] = map.emplace("a", 1);
    EXPECT_FALSE(success2);
    EXPECT_EQ(it2->first, "a");
}

TEST(UnorderedMap, Find) {
    mystd::unordered_map<const char *, int> map;
    map.emplace("a", 1);

    auto it = map.find("a");
    const auto cit = map.find("a");
    EXPECT_EQ(it, cit);
    EXPECT_EQ(it->first, "a");
    EXPECT_EQ(it->second, 1);

    auto dne_it = map.find("b");
    EXPECT_EQ(dne_it, map.end());
}

TEST(UnorderedMap, At) {
    mystd::unordered_map<const char *, int> map;
    map.emplace("a", 1);

    auto &data = map.at("a");
    EXPECT_EQ(data, 1);

    data = 2;
    const auto &const_data = map.at("a");
    EXPECT_EQ(const_data, 2);

    EXPECT_THROW(map.at("b"), std::out_of_range);
    EXPECT_THROW(map.at("b") = 2, std::out_of_range);
}

TEST(UnorderedMap, SubscriptOperator) {
    mystd::unordered_map<const char *, int> map;

    EXPECT_EQ(map["a"], 0);

    map["b"] = 1;
    EXPECT_EQ(map["b"], 1);
}
