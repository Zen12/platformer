#pragma once
#include "fsm_asset.hpp"
#include "node/node_serialization_yaml.hpp"
#include "connection/connection_serialization_yaml.hpp"
#include "condition/condition_serialization_yaml.hpp"
#include "yaml-cpp/node/node.h"


namespace YAML {
    template <>
    struct convert<FsmAsset> {
        static bool decode(const Node &node, FsmAsset &rhs) {
            // The YAML file format has each top level item as a list element
            if (node["name"]) {
                rhs.Name = node["name"].as<std::string>();
            }
            if (node["start_node"]) {
                rhs.StartNode = node["start_node"].as<std::string>();
            }
            if (node["nodes"]) {
                for (const auto &stateNode: node["nodes"]) {
                    auto nodeSer = stateNode.as<StateNodeSerialization>();
                    rhs.StateNodeSerialization.push_back(std::make_unique<StateNodeSerialization>(std::move(nodeSer)));
                }
            }
            if (node["connections"]) {
                rhs.ConnectionSerialization = node["connections"].as<std::vector<ConnectionSerialization>>();
            }
            if (node["conditions"]) {
                for (const auto &condition: node["conditions"]) {
                    auto condSer = condition.as<ConditionSerialization>();
                    rhs.ConditionSerialization.push_back(std::make_unique<ConditionSerialization>(std::move(condSer)));
                }
            }
            return true;
        }
    };
}


