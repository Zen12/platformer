
#pragma once
#include "fsm_serialization.hpp"
#include "condition/condition_serialization.hpp"
#include "condition/core_types/bool_check_condition_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<FsmSerialization> {

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
                { "bool_check",                [](const YAML::Node& n){ return Parse<BoolCheckConditionSerialization>(n); } }
            };

            if (const auto it = pairs.find(type); it != pairs.end()) {
                return it->second(node);
            }
            return {};
        }
        static bool decode(const Node &node, FsmSerialization &rhs) {
            for (const auto &condition: node["conditions"]) {
                rhs.Conditions.push_back(DecodeCondition(condition));
            }

            rhs.Nodes = node["nodes"].as<std::vector<StateNodeSerialization>>();
            rhs.Name = node["name"].as<std::string>();
            rhs.StartNode = node["start_node"].as<std::string>();


            return true;
        }
    };
}
