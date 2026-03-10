#pragma once
#include "fsm_asset.hpp"
#include "node/node_serialization_yaml.hpp"
#include "connection/connection_serialization_yaml.hpp"
#include "condition/condition_serialization.hpp"
#include "../register/fsm_condition_registry.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<FsmAsset> {
        static const FsmConditionRegistry* s_conditionRegistry;

        static std::unique_ptr<ConditionSerialization> DecodeCondition(const YAML::Node& node) {
            if (!s_conditionRegistry) return {};

            const auto nodeType = node["type"].as<std::string>();
            return s_conditionRegistry->Deserialize(nodeType, node);
        }

        static bool decode(const Node &node, FsmAsset &rhs) {
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
                    if (auto condSer = DecodeCondition(condition)) {
                        rhs.ConditionSerialization.push_back(std::move(condSer));
                    }
                }
            }
            return true;
        }
    };

    inline const FsmConditionRegistry* convert<FsmAsset>::s_conditionRegistry = nullptr;
}
