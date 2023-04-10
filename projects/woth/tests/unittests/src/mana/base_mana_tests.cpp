#include "woth/mana/base_mana.hpp"

#include "unity.hpp"

TEST_GROUP(mana_base_mana);
TEST_SETUP(mana_base_mana) {}
TEST_TEAR_DOWN(mana_base_mana) {}

TEST(mana_base_mana, is_snow_mana) {
    TEST_ASSERT_TRUE(::woth::is_snow(::woth::constants::mana::snow_colorless));
    TEST_ASSERT_TRUE(::woth::is_snow(::woth::constants::mana::snow_white));
    TEST_ASSERT_TRUE(::woth::is_snow(::woth::constants::mana::snow_blue));
    TEST_ASSERT_TRUE(::woth::is_snow(::woth::constants::mana::snow_black));
    TEST_ASSERT_TRUE(::woth::is_snow(::woth::constants::mana::snow_red));
    TEST_ASSERT_TRUE(::woth::is_snow(::woth::constants::mana::snow_green));

    TEST_ASSERT_FALSE(::woth::is_snow(::woth::constants::mana::colorless));
    TEST_ASSERT_FALSE(::woth::is_snow(::woth::constants::mana::white));
    TEST_ASSERT_FALSE(::woth::is_snow(::woth::constants::mana::blue));
    TEST_ASSERT_FALSE(::woth::is_snow(::woth::constants::mana::black));
    TEST_ASSERT_FALSE(::woth::is_snow(::woth::constants::mana::red));
    TEST_ASSERT_FALSE(::woth::is_snow(::woth::constants::mana::green));
}

TEST(mana_base_mana, is_color_mana) {
    TEST_ASSERT_TRUE(::woth::is_colorless(::woth::constants::mana::colorless));
    TEST_ASSERT_TRUE(::woth::is_colorless(::woth::constants::mana::snow_colorless));
    TEST_ASSERT_FALSE(::woth::is_colorless(::woth::constants::mana::blue));
    TEST_ASSERT_FALSE(::woth::is_colorless(::woth::constants::mana::snow_red));

    TEST_ASSERT_TRUE(::woth::is_white(::woth::constants::mana::white));
    TEST_ASSERT_TRUE(::woth::is_white(::woth::constants::mana::snow_white));
    TEST_ASSERT_FALSE(::woth::is_white(::woth::constants::mana::colorless));
    TEST_ASSERT_FALSE(::woth::is_white(::woth::constants::mana::snow_blue));

    TEST_ASSERT_TRUE(::woth::is_blue(::woth::constants::mana::blue));
    TEST_ASSERT_TRUE(::woth::is_blue(::woth::constants::mana::snow_blue));
    TEST_ASSERT_FALSE(::woth::is_blue(::woth::constants::mana::red));
    TEST_ASSERT_FALSE(::woth::is_blue(::woth::constants::mana::snow_colorless));

    TEST_ASSERT_TRUE(::woth::is_black(::woth::constants::mana::black));
    TEST_ASSERT_TRUE(::woth::is_black(::woth::constants::mana::snow_black));
    TEST_ASSERT_FALSE(::woth::is_black(::woth::constants::mana::green));
    TEST_ASSERT_FALSE(::woth::is_black(::woth::constants::mana::snow_white));

    TEST_ASSERT_TRUE(::woth::is_red(::woth::constants::mana::red));
    TEST_ASSERT_TRUE(::woth::is_red(::woth::constants::mana::snow_red));
    TEST_ASSERT_FALSE(::woth::is_red(::woth::constants::mana::colorless));
    TEST_ASSERT_FALSE(::woth::is_red(::woth::constants::mana::snow_black));

    TEST_ASSERT_TRUE(::woth::is_green(::woth::constants::mana::green));
    TEST_ASSERT_TRUE(::woth::is_green(::woth::constants::mana::snow_green));
    TEST_ASSERT_FALSE(::woth::is_green(::woth::constants::mana::red));
    TEST_ASSERT_FALSE(::woth::is_green(::woth::constants::mana::snow_blue));

    static constexpr auto hybrid_mask = static_cast<std::uint8_t>(::woth::detail::mana_mask_v<woth::color_t::blue> |
                                                                  woth::detail::mana_mask_v<woth::color_t::green>);
    static constexpr auto hybrid_mana = ::woth::base_mana_t(hybrid_mask);

    TEST_ASSERT_TRUE(::woth::is_blue(hybrid_mana));
    TEST_ASSERT_TRUE(::woth::is_green(hybrid_mana));
    TEST_ASSERT_FALSE(::woth::is_snow(hybrid_mana));
    TEST_ASSERT_FALSE(::woth::is_black(hybrid_mana));
}

TEST_GROUP_RUNNER(mana_base_mana) {
    RUN_TEST_CASE(mana_base_mana, is_snow_mana);
    RUN_TEST_CASE(mana_base_mana, is_color_mana);
}
