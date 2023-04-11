#ifndef JGE_MATH_MATH_HPP
#define JGE_MATH_MATH_HPP

#ifdef PSP
#include <fastmath.h>
#else
#include <math.h>
#endif

#include <bit>
#include <limits>
#include <cstdint>
#include <concepts>

namespace jge::math {

[[nodiscard]] inline auto sqrt(float number) noexcept -> float { return ::sqrtf(number); }
[[nodiscard]] inline auto sqrt(double number) noexcept -> double { return ::sqrt(number); }

template <std::floating_point OUT_T = double>
[[nodiscard]] inline auto sqrt(std::integral auto number) noexcept -> OUT_T {
    const auto f_num = static_cast<OUT_T>(number);
    return sqrt(f_num);
}

// NOTE: fast inv_sqrt (quake algorithm) https://mrober.io/papers/rsqrt.pdf
[[nodiscard]] inline constexpr auto inverse_sqrt(float number) noexcept -> float {
    static_assert(std::numeric_limits<float>::is_iec559);  // (enable only on IEEE 754)

    constexpr std::uint32_t magic = 0x5f375a86;

    const auto x2 = number * 0.5f;
    const auto y  = std::bit_cast<float>(magic - (std::bit_cast<std::uint32_t>(number) >> 1));
    return y * (1.5f - (x2 * y * y));
}

// NOTE: fast inv_sqrt (quake algorithm) https://mrober.io/papers/rsqrt.pdf
[[nodiscard]] inline constexpr auto inverse_sqrt(double number) noexcept -> double {
    static_assert(std::numeric_limits<double>::is_iec559);  // (enable only on IEEE 754)

    constexpr std::uint64_t magic = 0x5fe6eb50c7b537a9;

    const auto x2 = number * 0.5f;
    const auto y  = std::bit_cast<double>(magic - (std::bit_cast<std::uint64_t>(number) >> 1));
    return y * (1.5f - (x2 * y * y));
}

template <std::floating_point OUT_T = double>
[[nodiscard]] inline auto inverse_sqrt(std::integral auto number) noexcept -> OUT_T {
    const auto f_num = static_cast<OUT_T>(number);
    return inverse_sqrt(f_num);
}

[[nodiscard]] inline auto cos(float number) noexcept -> float { return ::cosf(number); }
[[nodiscard]] inline auto cos(double number) noexcept -> double { return ::cos(number); }

template <std::floating_point OUT_T = double>
[[nodiscard]] inline auto cos(std::integral auto number) noexcept -> OUT_T {
    const auto f_num = static_cast<OUT_T>(number);
    return cos(f_num);
}

[[nodiscard]] inline auto sin(float number) noexcept -> float { return ::sinf(number); }
[[nodiscard]] inline auto sin(double number) noexcept -> double { return ::sin(number); }

template <std::floating_point OUT_T = double>
[[nodiscard]] inline auto sin(std::integral auto number) noexcept -> OUT_T {
    const auto f_num = static_cast<OUT_T>(number);
    return sin(f_num);
}

[[nodiscard]] inline auto tan(float number) noexcept -> float { return ::tanf(number); }
[[nodiscard]] inline auto tan(double number) noexcept -> double { return ::tan(number); }

template <std::floating_point OUT_T = double>
[[nodiscard]] inline auto tan(std::integral auto number) noexcept -> OUT_T {
    const auto f_num = static_cast<OUT_T>(number);
    return tan(f_num);
}

[[nodiscard]] inline auto acos(float number) noexcept -> float { return ::acosf(number); }
[[nodiscard]] inline auto acos(double number) noexcept -> double { return ::acos(number); }

template <std::floating_point OUT_T = double>
[[nodiscard]] inline auto acos(std::integral auto number) noexcept -> OUT_T {
    const auto f_num = static_cast<OUT_T>(number);
    return acos(f_num);
}

[[nodiscard]] inline auto asin(float number) noexcept -> float { return ::asinf(number); }
[[nodiscard]] inline auto asin(double number) noexcept -> double { return ::asin(number); }

template <std::floating_point OUT_T = double>
[[nodiscard]] inline auto asin(std::integral auto number) noexcept -> OUT_T {
    const auto f_num = static_cast<OUT_T>(number);
    return asin(f_num);
}

[[nodiscard]] inline auto atan(float number) noexcept -> float { return ::atanf(number); }
[[nodiscard]] inline auto atan(double number) noexcept -> double { return ::atan(number); }

template <std::floating_point OUT_T = double>
[[nodiscard]] inline auto atan(std::integral auto number) noexcept -> OUT_T {
    const auto f_num = static_cast<OUT_T>(number);
    return atan(f_num);
}

[[nodiscard]] inline auto atan2(float x, float y) noexcept -> float { return ::atan2f(x, y); }
[[nodiscard]] inline auto atan2(double x, double y) noexcept -> double { return ::atan2(x, y); }

template <std::floating_point OUT_T = double>
[[nodiscard]] inline auto atan2(std::integral auto x, std::integral auto y) noexcept -> OUT_T {
    const auto x_f = static_cast<OUT_T>(x);
    const auto y_f = static_cast<OUT_T>(y);
    return atan2(x_f, y_f);
}

}  // namespace jge::math

#endif  // JGE_MATH_MATH_HPP
