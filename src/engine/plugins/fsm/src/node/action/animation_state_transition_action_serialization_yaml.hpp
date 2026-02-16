#pragma once
#include "animation_state_transition_action_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<AnimationStateTransitionActionSerialization> {
        static bool decode(const Node &node, AnimationStateTransitionActionSerialization &rhs) {
            rhs.FromAnimationGuid = node["from_animation_guid"].as<std::string>();
            rhs.ToAnimationGuid = node["to_animation_guid"].as<std::string>();
            rhs.TransitionTime = node["transition_time"].as<float>();
            if (node["on_complete_trigger"]) {
                rhs.OnCompleteTrigger = node["on_complete_trigger"].as<std::string>();
            }
            return true;
        }
    };
}
