#include "woth/string/algorithms.hpp"

#include "unity/unity.hpp"

#include <iostream>

TEST_GROUP(woth_string_algorithms_join);
TEST_SETUP(woth_string_algorithms_join) {}
TEST_TEAR_DOWN(woth_string_algorithms_join) {}

TEST(woth_string_algorithms_join, only_one_element) {
    const std::vector<std::string> expected = {"teststring"};

    const auto result = ::woth::join(expected);
    TEST_ASSERT_TRUE(("teststring " == result));
}

TEST(woth_string_algorithms_join, join_default_delimeter) {
    const std::vector<std::string> expected = {"test", "string"};

    const auto result = ::woth::join(expected);
    TEST_ASSERT_TRUE(("test string " == result));
}

TEST(woth_string_algorithms_join, join_dot_delimeter) {
    const std::vector<std::string> expected = {"test", "is", "a", "string"};

    const auto result = ::woth::join(expected, '.');
    TEST_ASSERT_TRUE(("test.is.a.string." == result));
}


TEST_GROUP_RUNNER(woth_string_algorithms_join) {
    RUN_TEST_CASE(woth_string_algorithms_join, only_one_element);
    RUN_TEST_CASE(woth_string_algorithms_join, join_default_delimeter);
    RUN_TEST_CASE(woth_string_algorithms_join, join_dot_delimeter);
}
