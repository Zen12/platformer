#pragma once
#include "fsm_asset.hpp"
#include "node/node_serialization_yaml.hpp"
#include "connection/connection_serialization_yaml.hpp"
#include "condition/condition_serialization.hpp"
#include "condition/core_types/trigger_check_condition_serialization_yaml.hpp"
#include "condition/core_types/always_true_condition_serialization_yaml.hpp"
#include "yaml-cpp/node/node.h"


namespace YAML {
    template <>
    struct convert<FsmAsset> {
        template<typename T>
        static std::unique_ptr<T> Parse(const YAML::Node &data) {
            static_assert(std::is_base_of_v<ConditionSerialization, T>,
                      "T must derive from ConditionSerialization");

            const auto action = data.as<T>();
            return std::make_unique<T>(action);
        }

        static std::unique_ptr<ConditionSerialization> DecodeCondition(const YAML::Node& node) {
            using type = std::function<std::unique_ptr<ConditionSerialization>(const YAML::Node&)>;
            static const std::unordered_map<std::string, type> pairs = {
                { "trigger_check", [](const YAML::Node& n){ return Parse<TriggerCheckConditionSerialization>(n); } },
                { "always_true",   [](const YAML::Node& n){ return Parse<AlwaysTrueConditionSerialization>(n); } }
            };

            const auto nodeType = node["type"].as<std::string>();
            if (const auto it = pairs.find(nodeType); it != pairs.end()) {
                return it->second(node);
            }
            return {};
        }

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
                    rhs.ConditionSerialization.push_back(DecodeCondition(condition));
                }
            }
            return true;
        }
    };
}


