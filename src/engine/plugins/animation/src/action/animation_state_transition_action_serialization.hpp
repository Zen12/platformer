#pragma once
#include <string>
#include "node/action/action_serialization.hpp"

class AnimationStateTransitionActionSerialization final : public ActionSerialization {
public:
    std::string FromAnimationGuid;
    std::string ToAnimationGuid;
    float TransitionTime;
    std::string OnCompleteTrigger;  // Optional trigger to set when transition completes

    ~AnimationStateTransitionActionSerialization() override = default;
};
