#pragma once
#include "always_true_condition_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<AlwaysTrueConditionSerialization> {

        static bool decode(const Node &node, AlwaysTrueConditionSerialization &rhs) {
            rhs.Type = node["type"].as<std::string>();
            rhs.Guid = node["guid"].as<std::string>();
            return true;
        }
    };
}
