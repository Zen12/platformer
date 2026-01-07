#pragma once
#include "health_check_action_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<HealthCheckActionSerialization>
    {
        static bool decode(const Node &node, HealthCheckActionSerialization &rhs)
        {
            rhs.TriggerName = node["trigger_name"].as<std::string>();
            return true;
        }
    };
}
