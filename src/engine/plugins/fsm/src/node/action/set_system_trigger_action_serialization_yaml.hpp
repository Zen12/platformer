#pragma once
#include "set_system_trigger_action_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<SetSystemTriggerActionSerialization> {
        static bool decode(const Node &node, SetSystemTriggerActionSerialization &rhs) {
            rhs.TriggerType = node["trigger_type"].as<std::string>();
            return true;
        }
    };
}
