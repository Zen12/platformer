#pragma once
#include "node_serialization.hpp"
#include "yaml-cpp/node/node.h"
#include "../action_registry.hpp"

namespace YAML {
    template <>
    struct convert<StateNodeSerialization> {
        static const ActionRegistry* s_actionRegistry;

        static std::unique_ptr<ActionSerialization> DecodeAction(const YAML::Node& node) {
            if (!s_actionRegistry) return {};

            const auto type = node["type"].as<std::string>();
            return s_actionRegistry->Deserialize(type, node);
        }

        static bool decode(const Node &node, StateNodeSerialization &rhs) {
            rhs.Guid = node["guid"].as<std::string>();
            if (node["actions"]) {
                for (const auto& action : node["actions"]) {
                    if (auto decoded = DecodeAction(action)) {
                        rhs.ActionData.push_back(std::move(decoded));
                    }
                }
            }
            return true;
        }
    };

    inline const ActionRegistry* convert<StateNodeSerialization>::s_actionRegistry = nullptr;
}
