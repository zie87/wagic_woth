#include "jge/math/math.hpp"

#include "unity/unity.hpp"

#include <iostream>

TEST_GROUP(jge_math_math);
TEST_SETUP(jge_math_math) {}
TEST_TEAR_DOWN(jge_math_math) {}

TEST(jge_math_math, sqrt) {
#if defined(UNITY_INCLUDE_DOUBLE)
    {
        constexpr double val = 36.0;
        const auto result    = ::jge::math::sqrt(val);
        TEST_ASSERT_DOUBLE_WITHIN(0.0001, 6.0, result);
    }
    {
        constexpr int val = 625;
        const auto result = ::jge::math::sqrt(val);

        TEST_ASSERT_TRUE((std::is_same_v<const double, decltype(result)>));
        TEST_ASSERT_DOUBLE_WITHIN(0.0001, 25.0, result);
    }
#endif

    {
        constexpr float val = 49.0;
        const auto result   = ::jge::math::sqrt(val);

        TEST_ASSERT_FLOAT_WITHIN(0.0001, 7.f, result);
    }
    {
        constexpr std::int16_t val = 121;
        const auto result          = ::jge::math::sqrt<float>(val);

        TEST_ASSERT_TRUE((std::is_same_v<const float, decltype(result)>));
        TEST_ASSERT_FLOAT_WITHIN(0.0001, 11.f, result);
    }
}

TEST(jge_math_math, inverse_sqrt) {
#if defined(UNITY_INCLUDE_DOUBLE)
    {
        constexpr double val = 16.0;
        const auto result    = ::jge::math::inverse_sqrt(val);
        TEST_ASSERT_DOUBLE_WITHIN(0.0001, 0.25, result);
    }
    {
        constexpr int val = 64;
        const auto result = ::jge::math::inverse_sqrt(val);

        TEST_ASSERT_TRUE((std::is_same_v<const double, decltype(result)>));
        TEST_ASSERT_DOUBLE_WITHIN(0.0001, 0.125f, result);
    }
#endif

    {
        constexpr float val = 256.0;
        const auto result   = ::jge::math::inverse_sqrt(val);

        TEST_ASSERT_FLOAT_WITHIN(0.001, 0.0625f, result);
    }
    {
        constexpr std::int16_t val = 100;
        const auto result          = ::jge::math::inverse_sqrt<float>(val);

        TEST_ASSERT_TRUE((std::is_same_v<const float, decltype(result)>));
        TEST_ASSERT_FLOAT_WITHIN(0.001, 0.1f, result);
    }
}

TEST_GROUP_RUNNER(jge_math_math) {
    RUN_TEST_CASE(jge_math_math, sqrt);
    RUN_TEST_CASE(jge_math_math, inverse_sqrt);
}
