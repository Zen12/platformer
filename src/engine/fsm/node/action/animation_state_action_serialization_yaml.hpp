#pragma once
#include "animation_state_action_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<AnimationStateActionSerialization> {
        static bool decode(const Node &node, AnimationStateActionSerialization &rhs) {
            rhs.AnimationGuid = node["animation_guid"].as<std::string>();
            if (node["on_complete_trigger"]) {
                rhs.OnCompleteTrigger = node["on_complete_trigger"].as<std::string>();
            }
            return true;
        }
    };
}
