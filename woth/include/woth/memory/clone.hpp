#ifndef WOTH_MEMORY_CLONE_HPP
#define WOTH_MEMORY_CLONE_HPP

#include <concepts>

namespace woth {
namespace concepts {

template <typename T>
concept cloneable = requires(T* ptr) {
    { ptr->clone() } -> std::convertible_to<T*>;
};

}  // namespace concepts

template <concepts::cloneable T>
[[nodiscard]] inline auto clone(const T* obj) -> T* {
    if (!obj) {
        return nullptr;
    }
    return obj->clone();
}

}  // namespace woth

#endif  // WOTH_MEMORY_CLONE_HPP
