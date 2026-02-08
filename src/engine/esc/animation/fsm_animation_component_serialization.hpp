#pragma once
#include "../../entity/component_serialization.hpp"
#include "../../system/guid.hpp"

struct FsmAnimationComponentSerialization final : public ComponentSerialization {
    Guid FsmGuid;
    bool Loop = true;
    float AnimationSpeed = 1.0f;
    bool VelocityBasedSpeed = false;
    float VelocitySpeedScale = 1.0f;

    Guid DirectionalWalkForwardGuid;
    Guid DirectionalWalkBackGuid;
    Guid DirectionalWalkLeftGuid;
    Guid DirectionalWalkRightGuid;
    bool UseDirectionalBlending = false;
};
