#pragma once
#include "health_display_action_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<HealthDisplayActionSerialization>
    {
        static bool decode(const Node &node, HealthDisplayActionSerialization &rhs)
        {
            rhs.ElementId = node["element_id"].as<std::string>();
            return true;
        }
    };
}
