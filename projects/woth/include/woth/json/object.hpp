#ifndef WOTH_JSON_OBJECT_HPP
#define WOTH_JSON_OBJECT_HPP

#include "woth/json/object_ref.hpp"
#include "woth/json/detail/cjson.hpp"
#include "woth/json/detail/json_object_ref.hpp"

#include <cjson/cJSON.h>

namespace woth::json {
class object final : public detail::json_object_ref<object> {
public:
    using handle_type = ::cJSON;
    using size_type   = std::size_t;

    using const_iterator = object_ref::const_iterator;

    object(const char* json_str) noexcept : m_object(detail::parse_json(json_str)) {}
    object(const std::string& json_str) noexcept : m_object(detail::parse_json(json_str)) {}
    ~object() noexcept { ::cJSON_Delete(m_object); }

    object(const object&)           = delete;
    object operator=(const object&) = delete;

    const handle_type* get_handle() const noexcept { return m_object; }

    object_ref operator[](const char* key) const noexcept { return detail::get_json_object(m_object, key); }
    object_ref operator[](const std::string& key) const noexcept { return detail::get_json_object(m_object, key); }
    object_ref operator[](size_type idx) const noexcept { return detail::get_json_child(m_object, idx); }

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
    handle_type* m_object = nullptr;
};
}  // namespace woth::json
#endif  // WOTH_JSON_OBJECT_HPP
