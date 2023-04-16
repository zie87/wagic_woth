#include "woth/string/algorithms/split.hpp"

#include <sstream>

namespace woth {

std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& container) {
    std::stringstream ss(s);
    std::string item{};
    while (std::getline(ss, item, delim)) {
        container.emplace_back(item);
    }
    return container;
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> vec{};
    return split(s, delim, vec);
}

}  // namespace woth
