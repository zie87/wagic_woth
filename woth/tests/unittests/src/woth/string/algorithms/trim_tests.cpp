#include "woth/string/algorithms.hpp"

#include "unity/unity.hpp"

#include <iostream>

TEST_GROUP(woth_string_algorithms_trim);
TEST_SETUP(woth_string_algorithms_trim) {}
TEST_TEAR_DOWN(woth_string_algorithms_trim) {}

TEST(woth_string_algorithms_trim, trim_left) {
    const std::string expected = "teststring";
    {
        std::string test_string = expected;
        ::woth::trim_left(test_string);
        TEST_ASSERT_TRUE((expected == test_string));
    }
    {
        std::string test_string = ' ' + expected;
        ::woth::trim_left(test_string);
        TEST_ASSERT_TRUE((expected == test_string));
    }
    {
        std::string test_string = '\t' + expected;
        ::woth::trim_left(test_string);
        TEST_ASSERT_TRUE((expected == test_string));
    }
    {
        std::string test_string = " \t\t " + expected;
        ::woth::trim_left(test_string);
        TEST_ASSERT_TRUE((expected == test_string));
    }
}

TEST(woth_string_algorithms_trim, trim_right) {
    const std::string expected = "teststring";
    {
        std::string test_string = expected;
        ::woth::trim_right(test_string);
        TEST_ASSERT_TRUE((expected == test_string));
    }
    {
        std::string test_string = expected + ' ';
        ::woth::trim_right(test_string);
        TEST_ASSERT_TRUE((expected == test_string));
    }
    {
        std::string test_string = expected + '\t';
        ::woth::trim_right(test_string);
        TEST_ASSERT_TRUE((expected == test_string));
    }
    {
        std::string test_string = expected + " \t\t ";
        ::woth::trim_right(test_string);
        TEST_ASSERT_TRUE((expected == test_string));
    }
}

TEST(woth_string_algorithms_trim, trim) {
    const std::string expected = "teststring";
    {
        std::string test_string = expected;
        ::woth::trim(test_string);
        TEST_ASSERT_TRUE((expected == test_string));
    }
    {
        std::string test_string = ' ' + expected;
        ::woth::trim(test_string);
        TEST_ASSERT_TRUE((expected == test_string));
    }
    {
        std::string test_string = expected + '\t';
        ::woth::trim(test_string);
        TEST_ASSERT_TRUE((expected == test_string));
    }
    {
        std::string test_string = "\t " + expected + " \t\t ";
        ::woth::trim(test_string);
        TEST_ASSERT_TRUE((expected == test_string));
    }
}

TEST(woth_string_algorithms_trim, trim_const) {

    const std::string expected = "teststring";
    {
        const std::string test_string = ' ' + expected;
        auto result = ::woth::trim(test_string);
        TEST_ASSERT_TRUE((expected == result));
    }
    {
        const std::string test_string = expected + '\t';
        auto result = ::woth::trim(test_string);
        TEST_ASSERT_TRUE((expected == result));
    }
    {
        const std::string test_string = '\t' + expected + ' ';
        auto result = ::woth::trim(test_string);
        TEST_ASSERT_TRUE((expected == result));
    }

}

TEST_GROUP_RUNNER(woth_string_algorithms_trim) {
    RUN_TEST_CASE(woth_string_algorithms_trim, trim_left);
    RUN_TEST_CASE(woth_string_algorithms_trim, trim_right);
    RUN_TEST_CASE(woth_string_algorithms_trim, trim);
    RUN_TEST_CASE(woth_string_algorithms_trim, trim_const);
}
