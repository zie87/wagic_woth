#ifndef WOTH_MANA_MANA_POOL_HPP
#define WOTH_MANA_MANA_POOL_HPP

#include "woth/mana/mana_type.hpp"

#include <array>

namespace woth {

class mana_value_pair {
public:
    constexpr mana_value_pair() noexcept = default;
    constexpr mana_value_pair(mana_type t, mana_value_type val) noexcept : m_type{t}, m_value{val} {}

    [[nodiscard]] constexpr inline auto type() const noexcept { return m_type; }
    [[nodiscard]] constexpr inline auto value() const noexcept { return m_value; }

private:
    mana_value_type m_value = 0;
    mana_type m_type        = mana_waste;
};

class base_mana_pool {
public:
    using pool_type = std::array<mana_value_type, detail::mana_type_count>;

    constexpr base_mana_pool() noexcept = default;

    template <mana_type T>
    [[nodiscard]] inline constexpr auto value() const noexcept -> mana_value_type {
        return m_pool[detail::to_index(T)];
    }

    inline constexpr void add(mana_value_type value, mana_type type) noexcept {
        m_pool[detail::to_index(type)] += value;
    }

    inline constexpr void remove(mana_value_type value, mana_type type) noexcept {
        m_pool[detail::to_index(type)] -= value;
    }

    [[nodiscard]] inline constexpr auto operator+(mana_value_pair pair) const noexcept -> base_mana_pool {
        base_mana_pool tmp(*this);
        tmp.add(pair.value(), pair.type());
        return tmp;
    }

    [[nodiscard]] inline constexpr auto operator+=(mana_value_pair pair) noexcept {
        (*this + pair).swap(*this);
        return this;
    }

    [[nodiscard]] inline constexpr auto operator-(mana_value_pair pair) const noexcept -> base_mana_pool {
        base_mana_pool tmp(*this);
        tmp.remove(pair.value(), pair.type());
        return tmp;
    }

    [[nodiscard]] inline constexpr auto operator-=(mana_value_pair pair) noexcept {
        (*this - pair).swap(*this);
        return this;
    }

    inline constexpr void swap(base_mana_pool& other) noexcept {
        using std::swap;
        swap(m_pool, other.m_pool);
    }

private:
    pool_type m_pool{};
};

};  // namespace woth

#endif  // WOTH_MANA_MANA_POOL_HPP
