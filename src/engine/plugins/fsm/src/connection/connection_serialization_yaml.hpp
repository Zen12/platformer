
#pragma once
#include "connection_serialization.hpp"
#include "yaml-cpp/node/node.h"
#include "ui/ui_page_action_serialization.hpp"

namespace YAML {
    template <>
    struct convert<ConnectionSerialization> {

        static bool decode(const Node &node, ConnectionSerialization &rhs) {
            rhs.Nodes = node["nodes"].as<std::array<std::string, 2>>();
            rhs.ConditionGuids = node["conditions"].as<std::vector<std::string>>();
            rhs.ConditionType = node["condition_type"].as<int>();
            return true;
        }
    };
}
