#pragma once
#include "bt_def.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {

static const std::vector<std::string> BT_FLOAT_KEYS = {
    "duration", "time", "radius", "range", "distance",
    "threshold", "count", "times", "param", "param2"
};

static const std::vector<std::string> BT_STRING_KEYS = {
    "tag"
};

inline void ParseNode(const Node& node, BehaviorTreeDef& tree, std::vector<uint16_t>& parentStack) {
    BTNodeDef nodeDef{};
    auto typeStr = node["type"].as<std::string>();
    nodeDef.Composite = BTCompositeFromString(typeStr);
    nodeDef.Type = (nodeDef.Composite == BTCompositeType::NONE) ? typeStr : "";

    for (const auto& key : BT_FLOAT_KEYS) {
        if (node[key]) nodeDef.Floats[key] = node[key].as<float>();
    }
    for (const auto& key : BT_STRING_KEYS) {
        if (node[key]) nodeDef.Strings[key] = node[key].as<std::string>();
    }

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
