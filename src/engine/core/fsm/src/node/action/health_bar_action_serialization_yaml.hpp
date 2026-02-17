#pragma once
#include "health_bar_action_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<HealthBarActionSerialization> {
        static bool decode(const Node &node, HealthBarActionSerialization &rhs) {
            rhs.ElementId = node["element_id"].as<std::string>();
            return true;
        }
    };
}
