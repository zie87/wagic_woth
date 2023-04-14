#include "woth/mana/mana_type.hpp"

#include "unity/unity.hpp"

#include <iostream>

TEST_GROUP(woth_mana_mana_type);
TEST_SETUP(woth_mana_mana_type) {}
TEST_TEAR_DOWN(woth_mana_mana_type) {}

TEST(woth_mana_mana_type, contains_mana_type) {
    {
        constexpr auto mana = woth::mana_waste;
        TEST_ASSERT_TRUE((woth::detail::contains_mana_type<woth::mana_waste>(mana)));
        TEST_ASSERT_FALSE((woth::detail::contains_mana_type<woth::mana_white>(mana)));
        TEST_ASSERT_FALSE((woth::detail::contains_mana_type<woth::mana_blue>(mana)));
        TEST_ASSERT_FALSE((woth::detail::contains_mana_type<woth::mana_black>(mana)));
        TEST_ASSERT_FALSE((woth::detail::contains_mana_type<woth::mana_red>(mana)));
        TEST_ASSERT_FALSE((woth::detail::contains_mana_type<woth::mana_green>(mana)));
    }
    {
        constexpr auto mana = woth::mana_blue;
        TEST_ASSERT_FALSE((woth::detail::contains_mana_type<woth::mana_waste>(mana)));
        TEST_ASSERT_FALSE((woth::detail::contains_mana_type<woth::mana_white>(mana)));
        TEST_ASSERT_TRUE((woth::detail::contains_mana_type<woth::mana_blue>(mana)));
        TEST_ASSERT_FALSE((woth::detail::contains_mana_type<woth::mana_black>(mana)));
        TEST_ASSERT_FALSE((woth::detail::contains_mana_type<woth::mana_red>(mana)));
        TEST_ASSERT_FALSE((woth::detail::contains_mana_type<woth::mana_green>(mana)));
    }
    {
        constexpr auto mana = woth::detail::create_mana_type({woth::mana_white, woth::mana_green});
        TEST_ASSERT_FALSE((woth::detail::contains_mana_type<woth::mana_waste>(mana)));
        TEST_ASSERT_TRUE((woth::detail::contains_mana_type<woth::mana_white>(mana)));
        TEST_ASSERT_FALSE((woth::detail::contains_mana_type<woth::mana_blue>(mana)));
        TEST_ASSERT_FALSE((woth::detail::contains_mana_type<woth::mana_black>(mana)));
        TEST_ASSERT_FALSE((woth::detail::contains_mana_type<woth::mana_red>(mana)));
        TEST_ASSERT_TRUE((woth::detail::contains_mana_type<woth::mana_green>(mana)));
    }
    {
        // clang-format off
        constexpr auto mana = woth::detail::create_mana_type({  woth::mana_white, 
                                                                woth::mana_blue, 
                                                                woth::mana_black, 
                                                                woth::mana_red, 
                                                                woth::mana_green });
        // clang-format on
        TEST_ASSERT_FALSE((woth::detail::contains_mana_type<woth::mana_waste>(mana)));
        TEST_ASSERT_TRUE((woth::detail::contains_mana_type<woth::mana_white>(mana)));
        TEST_ASSERT_TRUE((woth::detail::contains_mana_type<woth::mana_blue>(mana)));
        TEST_ASSERT_TRUE((woth::detail::contains_mana_type<woth::mana_black>(mana)));
        TEST_ASSERT_TRUE((woth::detail::contains_mana_type<woth::mana_red>(mana)));
        TEST_ASSERT_TRUE((woth::detail::contains_mana_type<woth::mana_green>(mana)));
    }
}

TEST(woth_mana_mana_type, mana_type_to_index) {
    TEST_ASSERT_EQUAL(0U, woth::detail::to_index(woth::mana_waste));
    TEST_ASSERT_EQUAL(1U, woth::detail::to_index(woth::mana_white));
    TEST_ASSERT_EQUAL(2U, woth::detail::to_index(woth::mana_blue));
    TEST_ASSERT_EQUAL(3U, woth::detail::to_index(woth::mana_black));
    TEST_ASSERT_EQUAL(4U, woth::detail::to_index(woth::mana_red));
    TEST_ASSERT_EQUAL(5U, woth::detail::to_index(woth::mana_green));
}

TEST(woth_mana_mana_type, mana_type_type_count) {
    {
        constexpr auto mana = woth::detail::create_mana_type({woth::mana_waste});

        TEST_ASSERT_EQUAL(1U, woth::detail::type_count(mana));
        TEST_ASSERT_TRUE((woth::detail::is_mono_type(mana)));
        TEST_ASSERT_FALSE((woth::detail::is_multi_type(mana)));
    }
    {
        constexpr auto mana = woth::detail::create_mana_type({woth::mana_red});

        TEST_ASSERT_EQUAL(1U, woth::detail::type_count(mana));
        TEST_ASSERT_TRUE((woth::detail::is_mono_type(mana)));
        TEST_ASSERT_FALSE((woth::detail::is_multi_type(mana)));
    }
    {
        constexpr auto mana = woth::detail::create_mana_type({woth::mana_black, woth::mana_red});

        TEST_ASSERT_EQUAL(2U, woth::detail::type_count(mana));
        TEST_ASSERT_FALSE((woth::detail::is_mono_type(mana)));
        TEST_ASSERT_TRUE((woth::detail::is_multi_type(mana)));
    }
    {
        constexpr auto mana = woth::detail::create_mana_type({woth::mana_white, woth::mana_black, woth::mana_red});

        TEST_ASSERT_EQUAL(3U, woth::detail::type_count(mana));
        TEST_ASSERT_FALSE((woth::detail::is_mono_type(mana)));
        TEST_ASSERT_TRUE((woth::detail::is_multi_type(mana)));
    }
}

TEST_GROUP_RUNNER(woth_mana_mana_type) {
    RUN_TEST_CASE(woth_mana_mana_type, contains_mana_type);
    RUN_TEST_CASE(woth_mana_mana_type, mana_type_to_index);
    RUN_TEST_CASE(woth_mana_mana_type, mana_type_type_count);
}
