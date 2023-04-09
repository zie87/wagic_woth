#ifndef WOTH_JSON_DETAIL_CJSON_HPP
#define WOTH_JSON_DETAIL_CJSON_HPP

#include <string>

#include <cjson/cJSON.h>

namespace woth::json::detail {

static inline ::cJSON* parse_json(const char* str, std::size_t len) noexcept {
    return ::cJSON_ParseWithLength(str, len);
}
static inline ::cJSON* parse_json(const std::string& str) noexcept { return parse_json(str.data(), str.size()); }

static inline bool has_json_object(const cJSON* const object, const char* key) noexcept {
    return ::cJSON_HasObjectItem(object, key);
}

static inline bool has_json_object(const cJSON* const object, const std::string& key) noexcept {
    return has_json_object(object, key.c_str());
}

static inline ::cJSON* get_json_object(const cJSON* const object, const char* key) noexcept {
    return ::cJSON_GetObjectItemCaseSensitive(object, key);
}

static inline ::cJSON* get_json_object(const cJSON* const object, const std::string& key) noexcept {
    return get_json_object(object, key.c_str());
}

static inline bool is_json_bool(const cJSON* const object) noexcept { return ::cJSON_IsBool(object); }
static inline bool is_json_number(const cJSON* const object) noexcept { return ::cJSON_IsNumber(object); }
static inline bool is_json_string(const cJSON* const object) noexcept { return ::cJSON_IsString(object); }
static inline bool is_json_array(const cJSON* const object) noexcept { return ::cJSON_IsArray(object); }
static inline bool is_json_object(const cJSON* const object) noexcept { return ::cJSON_IsObject(object); }
static inline bool is_json_raw(const cJSON* const object) noexcept { return ::cJSON_IsRaw(object); }

static inline std::size_t count_json_childs(const cJSON* const object) noexcept { return ::cJSON_GetArraySize(object); }
static inline ::cJSON* get_json_child(const cJSON* const object, std::size_t idx) noexcept {
    return ::cJSON_GetArrayItem(object, static_cast<int>(idx));
}

static inline double get_json_number(const cJSON* const object) noexcept { return ::cJSON_GetNumberValue(object); }
static inline const char* get_json_string(const cJSON* const object) noexcept { return ::cJSON_GetStringValue(object); }
}  // namespace woth::json::detail

#endif  // WOTH_JSON_DETAIL_CJSON_HPP
