#ifndef WOTH_MANA_MANA_TYPE_HPP
#define WOTH_MANA_MANA_TYPE_HPP

#include "woth/detail/enum_helper.hpp"

#include <initializer_list>
#include <functional>
#include <numeric>
#include <cstdint>
#include <bit>

namespace woth {
namespace detail {
static constexpr std::uint8_t mana_mask_waste = 0b00000001;
static constexpr std::uint8_t mana_mask_white = 0b00000010;
static constexpr std::uint8_t mana_mask_blue  = 0b00000100;
static constexpr std::uint8_t mana_mask_black = 0b00001000;
static constexpr std::uint8_t mana_mask_red   = 0b00010000;
static constexpr std::uint8_t mana_mask_green = 0b00100000;

static constexpr std::size_t mana_type_count = 6U;

}  // namespace detail

using mana_value_type = std::int16_t;
enum class mana_type : std::uint8_t;

namespace detail {
[[nodiscard]] constexpr inline auto create_mana_type(mana_type type) noexcept -> mana_type { return type; }
[[nodiscard]] constexpr inline auto create_mana_type(std::initializer_list<mana_type> types) noexcept -> mana_type {
    return std::accumulate(std::begin(types), std::end(types), mana_type(0),
                           [](mana_type lhs, mana_type rhs) -> mana_type {
                               return mana_type(underlying_cast(lhs) | underlying_cast(rhs));
                           });
}
}  // namespace detail

static constexpr mana_type mana_waste{detail::mana_mask_waste};
static constexpr mana_type mana_white{detail::mana_mask_white};
static constexpr mana_type mana_blue{detail::mana_mask_blue};
static constexpr mana_type mana_black{detail::mana_mask_black};
static constexpr mana_type mana_red{detail::mana_mask_red};
static constexpr mana_type mana_green{detail::mana_mask_green};

static constexpr mana_type mana_generic(detail::create_mana_type({woth::mana_waste, woth::mana_white, woth::mana_blue,
                                                                  woth::mana_black, woth::mana_red, woth::mana_green}));

namespace detail {
// clang-format off
template <mana_type T = mana_waste> 
struct mana_mask { static constexpr auto value = mana_mask_waste; };
template <> struct mana_mask<mana_white> { static constexpr auto value = mana_mask_white;};
template <> struct mana_mask<mana_blue>  { static constexpr auto value = mana_mask_blue; };
template <> struct mana_mask<mana_black> { static constexpr auto value = mana_mask_black; };
template <> struct mana_mask<mana_red>   { static constexpr auto value = mana_mask_red; };
template <> struct mana_mask<mana_green> { static constexpr auto value = mana_mask_green; };
// clang-format on

template <mana_type T>
inline constexpr auto mana_mask_v = mana_mask<T>::value;

template <mana_type T>
[[nodiscard]] inline constexpr bool contains_mana_type(mana_type mana) noexcept {
    return underlying_cast(mana) & mana_mask_v<T>;
}

[[nodiscard]] inline constexpr std::size_t type_count(mana_type mana) noexcept {
    const auto cnt = std::popcount(underlying_cast(mana));
    return static_cast<std::size_t>(cnt);
}

[[nodiscard]] inline constexpr bool is_generic(mana_type mana) noexcept { return mana == mana_generic; }
[[nodiscard]] inline constexpr bool is_mono_type(mana_type mana) noexcept { return type_count(mana) == 1; }
[[nodiscard]] inline constexpr bool is_multi_type(mana_type mana) noexcept { return !(is_mono_type(mana)); }

[[nodiscard]] inline constexpr std::size_t to_index(mana_type mana) noexcept {
    const auto val = underlying_cast(mana);
    const auto pos = std::countr_zero(val);
    return pos;
}
}  // namespace detail
}  // namespace woth

#endif  // WOTH_MANA_MANA_TYPE_HPP
