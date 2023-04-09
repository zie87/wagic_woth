#include "woth/json/json.hpp"

#include <string>
#include <stdio.h>

static const std::string monitor_json = R"(
{
    "name": "Awesome 4K",
    "resolutions": [
        {
            "width": 1280,
            "height": 720
        },
        {
            "width": 1920,
            "height": 1080
        },
        {
            "width": 3840,
            "height": 2160
        }
    ]
})";

bool supports_full_hd(const char* const monitor_str) {
    woth::json::object monitor(monitor_str);

    if (!monitor) {
        const char* error_ptr = ::cJSON_GetErrorPtr();
        if (error_ptr != nullptr) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return false;
    }
    printf("monitor child count: %zu \n", monitor.child_count());

    const auto name = monitor["name"];
    printf("name child count: %zu \n", name.child_count());
    if (name.is_string() && (name.get_string() != nullptr)) {
        printf("Checking monitor \"%s\"\n", name.get_string());
    }

    const auto resolutions = monitor["resolutions"];
    if (!resolutions.is_array()) {
        printf("wrong type for resolutions!\n");
        return false;
    }

    printf("resolutions child count: %zu \n", resolutions.child_count());
    for (const auto resolution : resolutions) {
        printf("resolution child count: %zu \n", resolution.child_count());

        const auto width  = resolution["width"];
        const auto height = resolution["height"];

        if (!width.is_number() || !height.is_number()) {
            return false;
        }

        printf("\twidth: %lf\n", width.get_number());
        printf("\theight: %lf\n", height.get_number());
    }

    return true;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    const auto ret = supports_full_hd(monitor_json.c_str());
    if (ret) {
        printf("parsing was successful!\n");
    } else {
        printf("failure could not parse!\n");
    }
}
