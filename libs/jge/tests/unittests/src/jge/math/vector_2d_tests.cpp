#include "jge/math/vector_2d.hpp"

#include "unity/unity.hpp"

#include <iostream>

TEST_GROUP(jge_math_vector_2d);
TEST_SETUP(jge_math_vector_2d) {}
TEST_TEAR_DOWN(jge_math_vector_2d) {}

TEST(jge_math_vector_2d, addition) {
    {
        jge::math::vector_2d vec1(1.f, 1.f);
        constexpr jge::math::vector_2d expected(4.f, 3.f);
        vec1 += jge::math::vector_2d(3.f, 2.f);

        TEST_ASSERT_TRUE((expected == vec1));
    }

    {
        jge::math::vector_2d vec1(3.f, 2.f);
        constexpr jge::math::vector_2d expected(-2.f, 3.f);
        vec1 += jge::math::vector_2d(-5.f, 1.f);

        TEST_ASSERT_TRUE((expected == vec1));
    }
}

TEST(jge_math_vector_2d, substraction) {
    {
        jge::math::vector_2d vec1(1.f, 1.f);
        constexpr jge::math::vector_2d expected(-2.f, -1.f);
        vec1 -= jge::math::vector_2d(3.f, 2.f);

        TEST_ASSERT_TRUE((expected == vec1));
    }

    {
        jge::math::vector_2d vec1(3.f, 2.f);
        constexpr jge::math::vector_2d expected(-2.f, 3.f);
        vec1 -= jge::math::vector_2d(5.f, -1.f);

        TEST_ASSERT_TRUE((expected == vec1));
    }
}

TEST(jge_math_vector_2d, multiplication) {
    {
        jge::math::vector_2d vec1(1.f, 1.f);
        constexpr jge::math::vector_2d expected(-3.f, -3.f);
        vec1 *= -3.f;

        TEST_ASSERT_TRUE((expected == vec1));
    }

    {
        jge::math::vector_2d vec1(3.f, 2.f);
        constexpr jge::math::vector_2d expected(6.f, 4.f);
        vec1 *= 2.f;

        TEST_ASSERT_TRUE((expected == vec1));
    }
    {
        constexpr jge::math::vector_2d expected(9.f, 12.f);
        const auto vec = 3.f * jge::math::vector_2d(3.f, 4.f);

        TEST_ASSERT_TRUE((expected == vec));
    }
}

TEST(jge_math_vector_2d, division) {
    {
        jge::math::vector_2d vec1(8.f, -6.f);
        constexpr jge::math::vector_2d expected(4.f, -3.f);
        vec1 /= 2.f;

        TEST_ASSERT_TRUE((expected == vec1));
    }

    {
        jge::math::vector_2d vec1(3.f, 2.f);
        constexpr jge::math::vector_2d expected(1.5f, 1.f);
        vec1 /= 2.f;

        TEST_ASSERT_TRUE((expected == vec1));
    }
}

TEST(jge_math_vector_2d, length) {
    {
        constexpr jge::math::vector_2d vec(4.f, -3.f);
        const auto length = vec.length();

        TEST_ASSERT_FLOAT_WITHIN(0.003f, 5.f, length);
    }
}

TEST(jge_math_vector_2d, angle) {
    {
        constexpr jge::math::vector_2d a(3.f, 4.f);
        constexpr jge::math::vector_2d b(1.f, 5.f);
        const auto angle = a.angle(b);

        TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.4461f, angle);
    }
}

TEST(jge_math_vector_2d, normalize) {
    constexpr auto norm_vec = jge::math::vector_2d(3.f, 4.f).normalize();
    TEST_ASSERT_FLOAT_WITHIN(0.003f, 1.f, norm_vec.length());
    TEST_ASSERT_FLOAT_WITHIN(0.003f, 0.6f, norm_vec.x());
    TEST_ASSERT_FLOAT_WITHIN(0.003f, 0.8f, norm_vec.y());
}

TEST_GROUP_RUNNER(jge_math_vector_2d) {
    RUN_TEST_CASE(jge_math_vector_2d, addition);
    RUN_TEST_CASE(jge_math_vector_2d, substraction);
    RUN_TEST_CASE(jge_math_vector_2d, multiplication);
    RUN_TEST_CASE(jge_math_vector_2d, division);
    RUN_TEST_CASE(jge_math_vector_2d, length);
    RUN_TEST_CASE(jge_math_vector_2d, angle);
    RUN_TEST_CASE(jge_math_vector_2d, normalize);
}
