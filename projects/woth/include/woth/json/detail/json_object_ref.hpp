#ifndef WOTH_JSON_DETAIL_JSON_OBJECT_REF_HPP
#define WOTH_JSON_DETAIL_JSON_OBJECT_REF_HPP

#include "woth/json/detail/cjson.hpp"

#include <cjson/cJSON.h>

namespace woth::json::detail {
template <typename DERIVED_T>
struct json_object_ref {
    using handle_type = ::cJSON;
    using size_type   = std::size_t;

    bool has_key(const char* key) const noexcept { return detail::has_json_object(handle(), key); }
    bool has_key(const std::string& key) const noexcept { return detail::has_json_object(handle(), key); }

    bool is_valid() const noexcept { return handle() != nullptr; }
    explicit operator bool() const noexcept { return is_valid(); }

    size_type child_count() const noexcept { return detail::count_json_childs(handle()); }

    inline bool is_object() const noexcept { return detail::is_json_object(handle()); }
    inline bool is_string() const noexcept { return detail::is_json_string(handle()); }
    inline bool is_number() const noexcept { return detail::is_json_number(handle()); }
    inline bool is_bool() const noexcept { return detail::is_json_bool(handle()); }
    inline bool is_array() const noexcept { return detail::is_json_array(handle()); }
    inline bool is_raw() const noexcept { return detail::is_json_raw(handle()); }

    inline double get_number() const noexcept { return detail::get_json_number(handle()); }
    inline const char* get_string() const noexcept { return detail::get_json_string(handle()); }

    friend const handle_type* to_handle(const json_object_ref& self) noexcept {
        return static_cast<const DERIVED_T&>(self).get_handle();
    }
    const handle_type* handle() const noexcept { return to_handle(*this); }

protected:
    ~json_object_ref() noexcept = default;
};
}  // namespace woth::json::detail
#endif  // WOTH_JSON_DETAIL_JSON_OBJECT_REF_HPP
