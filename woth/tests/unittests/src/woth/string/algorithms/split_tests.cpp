#include "woth/string/algorithms.hpp"

#include "unity/unity.hpp"

#include <iostream>

TEST_GROUP(woth_string_algorithms_split);
TEST_SETUP(woth_string_algorithms_split) {}
TEST_TEAR_DOWN(woth_string_algorithms_split) {}

TEST(woth_string_algorithms_split, split_no_delimiter) {
    const std::string expected = "teststring";

    const auto result = ::woth::split(expected, ',');
    TEST_ASSERT_EQUAL_UINT64(1U, (result.size()));
    TEST_ASSERT_TRUE((expected == result.front()));
}

TEST(woth_string_algorithms_split, split_space_delimiter) {
    const std::string expected = "test string";

    const auto result = ::woth::split(expected, ' ');
    TEST_ASSERT_EQUAL_UINT64(2U, (result.size()));
    TEST_ASSERT_TRUE(("test" == result.at(0)));
    TEST_ASSERT_TRUE(("string" == result.at(1)));
}

TEST_GROUP_RUNNER(woth_string_algorithms_split) {
    RUN_TEST_CASE(woth_string_algorithms_split, split_no_delimiter);
    RUN_TEST_CASE(woth_string_algorithms_split, split_space_delimiter);
}
