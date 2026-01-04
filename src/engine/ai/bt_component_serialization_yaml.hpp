#pragma once
#include "bt_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<BehaviorTreeComponentSerialization> {
        static bool decode(const Node& node, BehaviorTreeComponentSerialization& rhs) {
            if (node["tree"]) {
                rhs.TreeGuid = node["tree"].as<std::string>();
            }
            if (node["tick_priority"]) {
                rhs.TickPriority = node["tick_priority"].as<uint8_t>();
            }
            return true;
        }
    };
}
