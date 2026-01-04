#pragma once
#include "bt_asset.hpp"
#include "bt_serialization_yaml.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<BehaviorTreeAsset> {
        static bool decode(const Node& node, BehaviorTreeAsset& rhs) {
            rhs.Tree = node.as<BehaviorTreeDef>();
            return true;
        }
    };
}
