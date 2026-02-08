#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <utility>
#include "../../system/guid.hpp"

class FsmController;

class FsmAnimationComponent final {
public:
    Guid FsmGuid;
    Guid CurrentAnimationGuid;
    float Time = 0.0f;
    bool Loop = true;

    float AnimationSpeed = 1.0f;
    bool VelocityBasedSpeed = false;
    float VelocitySpeedScale = 1.0f;

    float StateAnimationSpeed = -1.0f;
    bool StateDisableVelocitySpeed = false;
    bool MovementDisabled = false;
    float MovementDisabledDuration = -1.0f;
    float MovementDisabledTimer = 0.0f;

    std::string OnCompleteTrigger;
    bool HasCompletedOnce = false;

    bool WasJumping = false;

    Guid DirectionalWalkForwardGuid;
    Guid DirectionalWalkBackGuid;
    Guid DirectionalWalkLeftGuid;
    Guid DirectionalWalkRightGuid;
    bool UseDirectionalBlending = false;

    bool IsTransitioning = false;
    Guid FromAnimationGuid;
    Guid ToAnimationGuid;
    float TransitionDuration = 0.0f;
    float TransitionProgress = 0.0f;

    std::shared_ptr<FsmController> Controller;
    std::shared_ptr<FsmAnimationComponent> SelfPtr;

    glm::vec3 EntityPosition = glm::vec3(0.0f);

    FsmAnimationComponent() = default;
    explicit FsmAnimationComponent(const Guid& fsmGuid)
        : FsmGuid(fsmGuid) {}
};
