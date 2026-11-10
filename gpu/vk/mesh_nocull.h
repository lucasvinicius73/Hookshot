// Work around task-shader culling hangs on the RADV devices where they have been reproduced.
#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string_view>

namespace mkwvk::mesh_nocull {

constexpr uint64_t kOotGridCullTask = 0x2c56dc1006942f74ull;

inline bool contains_ascii_case_insensitive(std::string_view text, std::string_view needle) {
    if (needle.empty() || needle.size() > text.size()) return false;
    const auto lower = [](char c) { return c >= 'A' && c <= 'Z' ? char(c - 'A' + 'a') : c; };
    for (size_t at = 0; at + needle.size() <= text.size(); at++) {
        size_t i = 0;
        while (i < needle.size() && lower(text[at + i]) == lower(needle[i])) i++;
        if (i == needle.size()) return true;
    }
    return false;
}

inline bool affected_radv_device(bool radv, std::string_view device_name) {
    if (!radv) return false;
    // RX 6600/6600 XT are Navi 23. RADV commonly exposes either the product name or the chip name.
    // The same shader hang was first reproduced on the Steam Deck's Vangogh GPU (issue 817).
    return contains_ascii_case_insensitive(device_name, "rx 6600") ||
           contains_ascii_case_insensitive(device_name, "navi23") ||
           contains_ascii_case_insensitive(device_name, "navi 23") ||
           contains_ascii_case_insensitive(device_name, "vangogh") ||
           contains_ascii_case_insensitive(device_name, "steam deck");
}

inline bool prefix_list_matches(uint64_t code_key, std::string_view list) {
    char hex[17];
    std::snprintf(hex, sizeof hex, "%016llx", (unsigned long long)code_key);
    for (size_t at = 0; at < list.size();) {
        const size_t comma = list.find(',', at);
        const std::string_view key = list.substr(at, comma == std::string_view::npos ? list.size() - at : comma - at);
        if (!key.empty() && key.size() <= 16 && !std::strncmp(hex, key.data(), key.size())) return true;
        at = comma == std::string_view::npos ? list.size() : comma + 1;
    }
    return false;
}

// An explicit setting always wins: "0" disables the workaround and any other value is a prefix list.
// With no setting, only the known hanging shader is changed, and only on affected RADV devices.
inline bool wanted(uint64_t code_key, const char* setting, bool affected_radv) {
    if (setting) return std::strcmp(setting, "0") != 0 && prefix_list_matches(code_key, setting);
    return affected_radv && code_key == kOotGridCullTask;
}

} // namespace mkwvk::mesh_nocull
