#pragma once
#include "bt_def.hpp"
#include <yaml-cpp/yaml.h>
#include <unordered_map>
#include <string>

namespace YAML {

inline BTNodeType StringToNodeType(const std::string& str) {
    static const std::unordered_map<std::string, BTNodeType> mapping = {
        // Composite
        {"sequence", BTNodeType::Sequence},
        {"selector", BTNodeType::Selector},
        {"parallel", BTNodeType::Parallel},
        // Decorator
        {"inverter", BTNodeType::Inverter},
        {"repeater", BTNodeType::Repeater},
        {"until_fail", BTNodeType::UntilFail},
        // Condition
        {"check_distance", BTNodeType::CheckDistance},
        {"has_target", BTNodeType::HasTarget},
        {"check_health", BTNodeType::CheckHealth},
        {"is_moving", BTNodeType::IsMoving},
        // Action
        {"move_to", BTNodeType::MoveTo},
        {"move_to_target", BTNodeType::MoveToTarget},
        {"attack", BTNodeType::Attack},
        {"idle", BTNodeType::Idle},
        {"wait", BTNodeType::Wait},
        {"random_wander", BTNodeType::RandomWander},
        {"set_target", BTNodeType::SetTarget},
        {"find_target_by_tag", BTNodeType::FindTargetByTag},
        {"has_target_in_range", BTNodeType::HasTargetInRange},
        {"clear_target", BTNodeType::ClearTarget},
    };

    auto it = mapping.find(str);
    return (it != mapping.end()) ? it->second : BTNodeType::Idle;
}

// Helper to read float parameter with type-specific field names (falls back to generic "param")
inline float ReadParam1(const Node& node, BTNodeType type) {
    // Type-specific field names
    switch (type) {
        case BTNodeType::Wait:
            if (node["duration"]) return node["duration"].as<float>();
            if (node["time"]) return node["time"].as<float>();
            break;
        case BTNodeType::RandomWander:
        case BTNodeType::FindTargetByTag:
            if (node["radius"]) return node["radius"].as<float>();
            break;
        case BTNodeType::CheckDistance:
        case BTNodeType::HasTargetInRange:
            if (node["range"]) return node["range"].as<float>();
            if (node["distance"]) return node["distance"].as<float>();
            break;
        case BTNodeType::CheckHealth:
            if (node["threshold"]) return node["threshold"].as<float>();
            break;
        case BTNodeType::Repeater:
            if (node["count"]) return node["count"].as<float>();
            if (node["times"]) return node["times"].as<float>();
            break;
        default:
            break;
    }
    // Generic fallback
    return node["param"] ? node["param"].as<float>() : 0.0f;
}

inline void ParseNode(const Node& node, BehaviorTreeDef& tree, std::vector<uint16_t>& parentStack) {
    BTNodeDef nodeDef{};
    nodeDef.Type = StringToNodeType(node["type"].as<std::string>());
    nodeDef.Param1 = ReadParam1(node, nodeDef.Type);
    nodeDef.Param2 = node["param2"] ? node["param2"].as<float>() : 0.0f;
    nodeDef.StringParam = node["tag"] ? node["tag"].as<std::string>() : "";
    nodeDef.ChildCount = 0;
    nodeDef.FirstChildIndex = 0;

    uint16_t thisIndex = static_cast<uint16_t>(tree.Nodes.size());
    tree.Nodes.push_back(nodeDef);

    // Process children if any
    if (node["children"] && node["children"].IsSequence()) {
        tree.Nodes[thisIndex].FirstChildIndex = static_cast<uint16_t>(tree.Nodes.size());
        tree.Nodes[thisIndex].ChildCount = static_cast<uint8_t>(node["children"].size());

        parentStack.push_back(thisIndex);
        for (const auto& child : node["children"]) {
            ParseNode(child, tree, parentStack);
        }
        parentStack.pop_back();
    }
}

template <>
struct convert<BehaviorTreeDef> {
    static bool decode(const Node& node, BehaviorTreeDef& rhs) {
        if (node["name"]) {
            rhs.Name = node["name"].as<std::string>();
        }

        if (node["root"]) {
            std::vector<uint16_t> parentStack;
            ParseNode(node["root"], rhs, parentStack);
        }

        return true;
    }
};

}
