#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class FsmAnimationComponent final {
public:
    std::string FsmGuid;  // GUID of the FSM that controls animation
    std::string CurrentAnimationGuid;  // Set by FSM action
    float Time = 0.0f;
    bool Loop = true;

    // Trigger to set when animation completes
    std::string OnCompleteTrigger;
    bool HasCompletedOnce = false;  // Track if non-looping animation has completed

    // Transition state
    bool IsTransitioning = false;
    std::string FromAnimationGuid;
    std::string ToAnimationGuid;
    float TransitionDuration = 0.0f;
    float TransitionProgress = 0.0f;

    bool ApplyRootMotion = false;
    std::string RootBoneName = "mixamorig:Hips_01";
    glm::vec3 PreviousRootPosition = glm::vec3(0.0f);
    glm::quat PreviousRootRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    bool FirstFrame = true;

    FsmAnimationComponent() = default;
    explicit FsmAnimationComponent(const std::string& fsmGuid)
        : FsmGuid(fsmGuid) {}
};
