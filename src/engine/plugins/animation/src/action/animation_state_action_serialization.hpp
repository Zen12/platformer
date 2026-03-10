#pragma once
#include <string>
#include "node/action/action_serialiazion.hpp"

class AnimationStateActionSerialization final : public ActionSerialization {
public:
    std::string AnimationGuid;
    std::string OnCompleteTrigger;  // Optional trigger to set when animation completes
    bool Loop = true;  // Whether to loop the animation (default: true)
    float AnimationSpeed = -1.0f;  // Override animation speed (-1 = use component default)
    bool DisableVelocitySpeed = false;  // Disable velocity-based speed for this state
    bool DisableMovement = false;  // Disable player movement during this state
    float DisableMovementDuration = -1.0f;  // How long to disable movement (-1 = until state exits)

    // Directional blending (4-way walk animations)
    bool UseDirectionalBlending = false;
    std::string DirectionalWalkForwardGuid;
    std::string DirectionalWalkBackGuid;
    std::string DirectionalWalkLeftGuid;
    std::string DirectionalWalkRightGuid;

    ~AnimationStateActionSerialization() override = default;
};
