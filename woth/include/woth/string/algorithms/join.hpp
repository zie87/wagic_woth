#ifndef WOTH_STRING_ALGORITHMS_JOIN_HPP
#define WOTH_STRING_ALGORITHMS_JOIN_HPP

#include <string>
#include <iterator>
#include <vector>

namespace woth {
inline std::string join(const std::vector<std::string>& v, char delim = ' ') {
    std::string val_str{};
    for (const auto& str : v) {
        val_str.append(str + delim);
    }
    return val_str;
}
}  // namespace woth

#endif  // WOTH_STRING_ALGORITHMS_JOIN_HPP
