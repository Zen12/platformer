#pragma once
#include <cstddef>
#include <functional>

enum class SystemTriggers {
    Exit = 0,
    Reload = 1,
};

namespace std {
    template <>
    struct hash<SystemTriggers> {
        std::size_t operator()(const SystemTriggers& trigger) const noexcept {
            return std::hash<int>{}(static_cast<int>(trigger));
        }
    };
}
