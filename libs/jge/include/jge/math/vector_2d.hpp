#include "jge/math/math.hpp"

#include <algorithm>

namespace jge::math {

template <typename VALUE_T>
class base_vector_2d {
public:
    using value_type = VALUE_T;

    constexpr base_vector_2d() noexcept = default;
    constexpr base_vector_2d(value_type x_val, value_type y_val) noexcept : m_x{x_val}, m_y{y_val} {}
    ~base_vector_2d() noexcept = default;

    constexpr base_vector_2d(const base_vector_2d& other) noexcept : m_x{other.m_x}, m_y{other.m_y} {}
    inline constexpr auto operator=(const base_vector_2d& other) noexcept -> base_vector_2d& {
        base_vector_2d(other).swap(*this);
        return *this;
    }

    constexpr base_vector_2d(base_vector_2d&& other) noexcept : m_x{std::move(other.m_x)}, m_y{std::move(other.m_y)} {}
    inline constexpr auto operator=(base_vector_2d&& other) noexcept -> base_vector_2d& {
        base_vector_2d(std::move(other)).swap(*this);
        return *this;
    }

    [[nodiscard]] inline constexpr auto x() const noexcept -> value_type { return m_x; }
    [[nodiscard]] inline constexpr auto x() noexcept -> value_type& { return m_x; }
    [[nodiscard]] inline constexpr auto y() const noexcept -> value_type { return m_y; }
    [[nodiscard]] inline constexpr auto y() noexcept -> value_type& { return m_y; }

    inline constexpr void swap(base_vector_2d& other) noexcept {
        using std::swap;
        swap(m_x, other.m_x);
        swap(m_y, other.m_y);
    }

    [[nodiscard]] inline constexpr auto operator+(const base_vector_2d& other) const noexcept -> base_vector_2d {
        return base_vector_2d((m_x + other.m_x), (m_y + other.m_y));
    }
    inline constexpr auto operator+=(const base_vector_2d& other) noexcept -> base_vector_2d& {
        (*this + other).swap(*this);
        return *this;
    }

    [[nodiscard]] inline constexpr auto operator-(const base_vector_2d& other) const noexcept -> base_vector_2d {
        return base_vector_2d((m_x - other.m_x), (m_y - other.m_y));
    }
    inline constexpr auto operator-=(const base_vector_2d& other) noexcept -> base_vector_2d& {
        (*this - other).swap(*this);
        return *this;
    }

    [[nodiscard]] inline constexpr auto operator*(value_type scalar) const noexcept -> base_vector_2d {
        return base_vector_2d((m_x * scalar), (m_y * scalar));
    }
    inline constexpr auto operator*=(value_type scalar) noexcept -> base_vector_2d& {
        (*this * scalar).swap(*this);
        return *this;
    }

    [[nodiscard]] inline constexpr auto operator/(value_type scalar) const noexcept -> base_vector_2d {
        return base_vector_2d((m_x / scalar), (m_y / scalar));
    }
    inline constexpr auto operator/=(value_type scalar) noexcept -> base_vector_2d& {
        (*this / scalar).swap(*this);
        return *this;
    }

    [[nodiscard]] inline constexpr auto operator*(const base_vector_2d& other) const noexcept -> value_type {
        return dot(other);
    }
    [[nodiscard]] inline constexpr auto dot(const base_vector_2d& other) const noexcept -> value_type {
        return (m_x * other.m_x) + (m_y * other.m_y);
    }

    [[nodiscard]] inline constexpr auto operator^(const base_vector_2d& other) const noexcept -> value_type {
        return cross(other);
    }
    [[nodiscard]] inline constexpr auto cross(const base_vector_2d& other) const noexcept -> value_type {
        return (m_x * other.m_y) - (m_y * other.m_x);
    }

    [[nodiscard]] inline auto length() const noexcept { return ::jge::math::sqrt(dot(*this)); }

    inline constexpr void clamp(const base_vector_2d& min, const base_vector_2d& max) noexcept {
        m_x = std::clamp(m_x, min.m_x, max.m_x);
        m_y = std::clamp(m_y, min.m_y, max.m_y);
    }

    inline constexpr auto normalize() noexcept -> base_vector_2d& {
        const auto factor = ::jge::math::inverse_sqrt(dot(*this));
        return (*this *= factor);
    }

    inline auto rotate(value_type angle) noexcept -> base_vector_2d& {
        const auto sin_val = ::jge::math::sin(angle);
        const auto cos_val = ::jge::math::cos(angle);

        const auto x_val = m_x * cos_val - m_y * sin_val;
        const auto y_val = m_x * sin_val - m_y * cos_val;

        return (*this = base_vector_2d(x_val, y_val));
    }

    inline auto rotate(const base_vector_2d& center, value_type angle) noexcept -> base_vector_2d& {
        base_vector_2d d = (*this - center).rotate(angle);
        return (*this = (center + d));
    }
    
    [[nodiscard]] inline auto angle() const noexcept -> value_type {
        return ::jge::math::atan2(m_y, m_x);
    }
    
    [[nodiscard]] inline auto angle(const base_vector_2d& other) const noexcept -> value_type {
        const auto d = dot(other);
        const auto c = cross(other);

        return ::jge::math::atan2(c, d);
    }
    /*
    Vector2D Vector2D::Direction(void) const {
        Vector2D temp(*this);

        temp.Normalize();

        return temp;
    }
    */

private:
    value_type m_x{};
    value_type m_y{};
};

template <typename V_T>
inline constexpr void swap(base_vector_2d<V_T>& lhs, base_vector_2d<V_T>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename V_T>
inline constexpr bool operator==(const base_vector_2d<V_T>& lhs, const base_vector_2d<V_T>& rhs) noexcept {
    return (lhs.x() == rhs.x()) && (lhs.y() == rhs.y());
}

template <typename V_T>
inline constexpr bool operator!=(const base_vector_2d<V_T>& lhs, const base_vector_2d<V_T>& rhs) noexcept {
    return !(lhs == rhs);
}

template <typename V_T>
inline constexpr auto operator*(V_T scala, const base_vector_2d<V_T>& vec) noexcept {
    return vec * scala;
}

using vector_2d = base_vector_2d<float>;

}  // namespace jge::math
