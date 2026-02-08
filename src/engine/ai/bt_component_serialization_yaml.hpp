#pragma once
#include "bt_component_serialization.hpp"
#include <yaml-cpp/yaml.h>
#include "../system/guid_yaml.hpp"

namespace YAML {
    template <>
    struct convert<BehaviorTreeComponentSerialization> {
        static bool decode(const Node& node, BehaviorTreeComponentSerialization& rhs) {
            if (node["tree"]) {
                rhs.TreeGuid = node["tree"].as<Guid>();
            }
            if (node["tick_priority"]) {
                rhs.TickPriority = node["tick_priority"].as<uint8_t>();
            }
            return true;
        }
    };
}
