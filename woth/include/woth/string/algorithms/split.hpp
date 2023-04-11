#ifndef WOTH_STRING_ALGORITHMS_SPLIT_HPP
#define WOTH_STRING_ALGORITHMS_SPLIT_HPP

#include <string>
#include <vector>

namespace woth {

std::vector<std::string> split(const std::string& s, char delim);
std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& container);

}  // namespace woth

#endif  // WOTH_STRING_ALGORITHMS_SPLIT_HPP
