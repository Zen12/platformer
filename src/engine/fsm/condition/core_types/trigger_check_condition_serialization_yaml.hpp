
#pragma once
#include "trigger_check_condition_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<TriggerCheckConditionSerialization> {

        static bool decode(const Node &node, TriggerCheckConditionSerialization &rhs) {
            rhs.Type = node["type"].as<std::string>();
            rhs.Guid = node["guid"].as<std::string>();
            rhs.TriggerName = node["trigger_name"].as<std::string>();
            std::cout << "TriggerCheckConditionSerialization" << std::endl;
            return true;
        }
    };
}
