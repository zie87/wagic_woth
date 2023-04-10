#ifndef WOTH_MANA_BASE_MANA_HPP
#define WOTH_MANA_BASE_MANA_HPP

#include "woth/basics/colors.hpp"

#include <cstdint>

namespace woth {

enum class base_mana_t : std::uint8_t {};

namespace constants::mana {
static constexpr std::uint8_t mask_colorless = 0x01;
static constexpr std::uint8_t mask_white     = 0x02;
static constexpr std::uint8_t mask_blue      = 0x04;
static constexpr std::uint8_t mask_black     = 0x08;
static constexpr std::uint8_t mask_red       = 0x10;
static constexpr std::uint8_t mask_green     = 0x20;
static constexpr std::uint8_t mask_snow      = 0x80;

static constexpr base_mana_t colorless = base_mana_t(mask_colorless);
static constexpr base_mana_t white     = base_mana_t(mask_white);
static constexpr base_mana_t blue      = base_mana_t(mask_blue);
static constexpr base_mana_t black     = base_mana_t(mask_black);
static constexpr base_mana_t red       = base_mana_t(mask_red);
static constexpr base_mana_t green     = base_mana_t(mask_green);

static constexpr base_mana_t snow_colorless = base_mana_t(mask_snow | mask_colorless);
static constexpr base_mana_t snow_white     = base_mana_t(mask_snow | mask_white);
static constexpr base_mana_t snow_blue      = base_mana_t(mask_snow | mask_blue);
static constexpr base_mana_t snow_black     = base_mana_t(mask_snow | mask_black);
static constexpr base_mana_t snow_red       = base_mana_t(mask_snow | mask_red);
static constexpr base_mana_t snow_green     = base_mana_t(mask_snow | mask_green);
}  // namespace constants::mana

namespace detail {
template <::woth::color_t>
struct mana_mask;

template <>
struct mana_mask<::woth::color_t::colorless> {
    static constexpr auto value = ::woth::constants::mana::mask_colorless;
};
template <>
struct mana_mask<::woth::color_t::white> {
    static constexpr auto value = ::woth::constants::mana::mask_white;
};
template <>
struct mana_mask<::woth::color_t::blue> {
    static constexpr auto value = ::woth::constants::mana::mask_blue;
};
template <>
struct mana_mask<::woth::color_t::black> {
    static constexpr auto value = ::woth::constants::mana::mask_black;
};
template <>
struct mana_mask<::woth::color_t::red> {
    static constexpr auto value = ::woth::constants::mana::mask_red;
};
template <>
struct mana_mask<::woth::color_t::green> {
    static constexpr auto value = ::woth::constants::mana::mask_green;
};

template <::woth::color_t COLOR_T>
inline constexpr auto mana_mask_v = mana_mask<COLOR_T>::value;
}  // namespace detail

[[nodiscard]] inline constexpr bool is_snow(base_mana_t mana) noexcept {
    return static_cast<unsigned int>(mana) & static_cast<unsigned int>(constants::mana::mask_snow);
}

template <::woth::color_t COLOR_T>
[[nodiscard]] inline constexpr bool is_color(base_mana_t mana) noexcept {
    return static_cast<unsigned int>(mana) & static_cast<unsigned int>(detail::mana_mask_v<COLOR_T>);
}

// clang-format off
[[nodiscard]] inline constexpr bool is_colorless(base_mana_t mana) noexcept {return is_color<::woth::color_t::colorless>(mana);}
[[nodiscard]] inline constexpr bool is_white(base_mana_t mana) noexcept {return is_color<::woth::color_t::white>(mana);}
[[nodiscard]] inline constexpr bool is_blue(base_mana_t mana) noexcept {return is_color<::woth::color_t::blue>(mana);}
[[nodiscard]] inline constexpr bool is_black(base_mana_t mana) noexcept {return is_color<::woth::color_t::black>(mana);}
[[nodiscard]] inline constexpr bool is_red(base_mana_t mana) noexcept {return is_color<::woth::color_t::red>(mana);}
[[nodiscard]] inline constexpr bool is_green(base_mana_t mana) noexcept {return is_color<::woth::color_t::green>(mana);}
// clang-format on

}  // namespace woth

#endif  // WOTH_MANA_BASE_MANA_HPP
