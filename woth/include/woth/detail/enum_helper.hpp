#ifndef WOTH_DETAIL_ENUM_HELPER_HPP
#define WOTH_DETAIL_ENUM_HELPER_HPP

#include <type_traits>

namespace woth::detail {
template <typename E, typename = typename std::is_enum<E>::type>
struct safe_underlying {
    using type = E;
};

template <typename E>
struct safe_underlying<E, std::true_type> {
    using type = std::underlying_type_t<E>;
};

template <typename E>
using safe_underlying_t = typename safe_underlying<E>::type;

template <typename E>
[[nodiscard]] inline constexpr auto underlying_cast(E e) noexcept -> safe_underlying_t<E> {
    return static_cast<safe_underlying_t<E>>(e);
}
}  // namespace woth::detail
#endif  // WOTH_DETAIL_ENUM_HELPER_HPP
