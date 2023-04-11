#ifndef WOTH_STRING_ALGORITHMS_TRIM_HPP
#define WOTH_STRING_ALGORITHMS_TRIM_HPP

#include <string>

namespace woth {

inline std::string& trim_left(std::string& str) noexcept {
    str.erase(0, str.find_first_not_of(" \t"));
    return str;
}

inline std::string& trim_right(std::string& str) noexcept {
    str.resize(str.find_last_not_of(" \t") + 1);
    return str;
}

inline std::string& trim(std::string& str) noexcept {
    trim_left(str);
    trim_right(str);
    return str;
}

inline std::string trim(const std::string& str) noexcept {
    std::string value(str);
    return trim(value);
}

}  // namespace woth

#endif  // WOTH_STRING_ALGORITHMS_TRIM_HPP
