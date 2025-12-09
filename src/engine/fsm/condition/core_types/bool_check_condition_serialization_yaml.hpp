
#pragma once
#include "bool_check_condition_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<BoolCheckConditionSerialization> {

        static bool decode(const Node &node, BoolCheckConditionSerialization &rhs) {
            rhs.Type = node["type"].as<std::string>();
            rhs.Guid = node["guid"].as<std::string>();
            rhs.Value = node["value"].as<bool>();
            rhs.ValueName = node["value_name"].as<std::string>();
            return true;
        }
    };
}
