#pragma once
#include <string>
#include "action_serialiazion.hpp"

class AnimationStateActionSerialization final : public ActionSerialization {
public:
    std::string AnimationGuid;
    std::string OnCompleteTrigger;  // Optional trigger to set when animation completes

    ~AnimationStateActionSerialization() override = default;
};
