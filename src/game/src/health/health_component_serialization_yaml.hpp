#pragma once
#include "health_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<HealthComponentSerialization> {
        static bool decode(const Node& node, HealthComponentSerialization& rhs) {
            rhs.MaxHealth = node["max_health"] ? node["max_health"].as<float>() : 100.0f;
            return true;
        }
    };
}
