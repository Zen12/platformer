#pragma once
#include "bt_def.hpp"
#include <yaml-cpp/yaml.h>
#include <unordered_set>
#include <string>

namespace YAML {

inline float ReadParam1(const Node& node, const std::string& type) {
    static const std::unordered_set<std::string> durationTypes = {"wait", "attack"};
    static const std::unordered_set<std::string> radiusTypes = {"random_wander", "find_target_by_tag", "idle"};
    static const std::unordered_set<std::string> rangeTypes = {"check_distance", "has_target_in_range"};

    if (durationTypes.count(type)) {
        if (node["duration"]) return node["duration"].as<float>();
        if (node["time"]) return node["time"].as<float>();
    } else if (radiusTypes.count(type)) {
        if (node["radius"]) return node["radius"].as<float>();
    } else if (rangeTypes.count(type)) {
        if (node["range"]) return node["range"].as<float>();
        if (node["distance"]) return node["distance"].as<float>();
    } else if (type == "check_health") {
        if (node["threshold"]) return node["threshold"].as<float>();
    } else if (type == "repeater") {
        if (node["count"]) return node["count"].as<float>();
        if (node["times"]) return node["times"].as<float>();
    }

    return node["param"] ? node["param"].as<float>() : 0.0f;
}

inline void ParseNode(const Node& node, BehaviorTreeDef& tree, std::vector<uint16_t>& parentStack) {
    BTNodeDef nodeDef{};
    nodeDef.Type = node["type"].as<std::string>();
    nodeDef.Param1 = ReadParam1(node, nodeDef.Type);
    nodeDef.Param2 = node["param2"] ? node["param2"].as<float>() : 0.0f;
    nodeDef.StringParam = node["tag"] ? node["tag"].as<std::string>() : "";
    nodeDef.ChildCount = 0;
    nodeDef.FirstChildIndex = 0;

    uint16_t thisIndex = static_cast<uint16_t>(tree.Nodes.size());
    tree.Nodes.push_back(nodeDef);

    if (!parentStack.empty()) {
        tree.Nodes[parentStack.back()].ChildIndices.push_back(thisIndex);
    }

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
