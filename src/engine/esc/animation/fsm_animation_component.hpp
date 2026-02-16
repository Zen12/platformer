#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../../system/guid.hpp"
#include "animation_blender.hpp"
#include "animation_soa_types.hpp"

class FsmController;

class FsmAnimationComponent final {
public:
    Guid FsmGuid;
    Guid CurrentAnimationGuid;
    float Time = 0.0f;
    bool Loop = true;

    float StateAnimationSpeed = -1.0f;
    bool StateDisableVelocitySpeed = false;
    bool MovementDisabled = false;
    float MovementDisabledDuration = -1.0f;
    float MovementDisabledTimer = 0.0f;

    std::string OnCompleteTrigger;
    bool HasCompletedOnce = false;

    bool WasJumping = false;

    bool IsTransitioning = false;
    Guid FromAnimationGuid;
    Guid ToAnimationGuid;
    float TransitionDuration = 0.2f;
    float TransitionProgress = 0.0f;

    std::shared_ptr<FsmController> Controller;
    std::shared_ptr<FsmAnimationComponent> SelfPtr;

    glm::vec3 EntityPosition = glm::vec3(0.0f);

    AnimationBlender Blender;
    BoneSoA CurrentBones;
    BoneSoA TargetBones;
    BoneSoA BlendedBones;

    FsmAnimationComponent() = default;
    explicit FsmAnimationComponent(const Guid& fsmGuid)
        : FsmGuid(fsmGuid) {}
};
