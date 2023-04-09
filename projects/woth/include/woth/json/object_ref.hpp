#ifndef WOTH_JSON_OBJECT_REF_HPP
#define WOTH_JSON_OBJECT_REF_HPP

#include "woth/json/detail/cjson.hpp"
#include "woth/json/detail/json_object_ref.hpp"

#include <cjson/cJSON.h>

namespace woth::json {
class object_ref final : public detail::json_object_ref<object_ref> {
public:
    using handle_type = ::cJSON;
    using size_type   = std::size_t;

    object_ref() noexcept = default;
    object_ref(const handle_type* obj) noexcept : m_object(obj) {}

    const handle_type* get_handle() const noexcept { return m_object; }

    object_ref operator[](const char* key) const noexcept { return detail::get_json_object(m_object, key); }
    object_ref operator[](const std::string& key) const noexcept { return detail::get_json_object(m_object, key); }
    object_ref operator[](size_type idx) const noexcept { return detail::get_json_child(m_object, idx); }

    class const_iterator {
    public:
        const_iterator() noexcept = default;
        const_iterator(const handle_type* child) noexcept : m_item(child) {}

        const_iterator& operator++() noexcept {
            next();
            return *this;
        }

        object_ref operator*() const noexcept { return {m_item}; }

        friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) noexcept {
            return lhs.m_item == rhs.m_item;
        }

        friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(lhs == rhs); }

    private:
        void next() noexcept {
            if (m_item != nullptr) {
                m_item = m_item->next;
            }
        }
        const handle_type* m_item = nullptr;
    };

    const_iterator begin() const noexcept { return cbegin(); }
    const_iterator end() const noexcept { return cend(); }

    const_iterator cbegin() const noexcept {
        if (is_valid()) {
            return {m_object->child};
        }
        return end();
    }

    const_iterator cend() const noexcept { return {}; }

private:
    const handle_type* m_object = nullptr;
};
}  // namespace woth::json
#endif  // WOTH_JSON_OBJECT_REF_HPP
