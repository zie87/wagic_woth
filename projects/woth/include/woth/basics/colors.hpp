#ifndef WOTH_BASICS_COLORS_HPP
#define WOTH_BASICS_COLORS_HPP

#include <cstdint>

namespace woth {

enum class color_t : std::uint8_t {
    colorless = 0x01,
    white     = 0x02,
    blue      = 0x04,
    black     = 0x08,
    red       = 0x10,
    green     = 0x20,
};

}  // namespace woth

#endif  // WOTH_BASICS_COLORS_HPP
