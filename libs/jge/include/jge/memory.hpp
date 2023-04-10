#ifndef JGE_MEMORY_HPP
#define JGE_MEMORY_HPP

#include <memory>

namespace jge {

template <typename T>
using shared_ptr = std::shared_ptr<T>;

}  // namespace jge

#endif  // JGE_MEMORY_HPP
