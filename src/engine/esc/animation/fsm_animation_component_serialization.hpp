#pragma once
#include "../../entity/component_serialization.hpp"
#include <string>

struct FsmAnimationComponentSerialization final : public ComponentSerialization {
    std::string FsmGuid;
    bool Loop = true;
    float AnimationSpeed = 1.0f;
    bool VelocityBasedSpeed = false;
    float VelocitySpeedScale = 1.0f;

    // Directional animation blending
    std::string DirectionalWalkForwardGuid;
    std::string DirectionalWalkBackGuid;
    std::string DirectionalWalkLeftGuid;
    std::string DirectionalWalkRightGuid;
    bool UseDirectionalBlending = false;
};
