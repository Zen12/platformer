#pragma once
#include "fps_display_action_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<FpsDisplayActionSerialization>
    {
        static bool decode(const Node &node, FpsDisplayActionSerialization &rhs)
        {
            rhs.ElementId = node["element_id"].as<std::string>();
            return true;
        }
    };
}
