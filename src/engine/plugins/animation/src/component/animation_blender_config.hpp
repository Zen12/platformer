#pragma once
#include "animation_blend_mode.hpp"
#include "guid.hpp"

struct AnimationBlenderConfig {
    AnimationBlendMode Mode = AnimationBlendMode::Simple;
    float TransitionDuration = 0.2f;
    float AnimationSpeed = 1.0f;
    bool VelocityBasedSpeed = false;
    float VelocitySpeedScale = 1.0f;

    Guid DirectionalForwardGuid;
    Guid DirectionalBackGuid;
    Guid DirectionalLeftGuid;
    Guid DirectionalRightGuid;
};
