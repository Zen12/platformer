#pragma once
#include "action.hpp"
#include "../../../esc/animation/fsm_animation_component.hpp"
#include <memory>
#include <string>
#include <iostream>

#define DEBUG_ANIM_STATE_ACTION 0

#if DEBUG_ANIM_STATE_ACTION
#define ANIM_STATE_LOG if(1) std::cout
#else
#define ANIM_STATE_LOG if(0) std::cout
#endif

struct AnimationStateAction final : public Action {
private:
    std::string _animationGuid;
    std::string _onCompleteTrigger;
    bool _loop;
    float _animationSpeed;
    bool _disableVelocitySpeed;
    bool _disableMovement;
    float _disableMovementDuration;
    std::weak_ptr<FsmAnimationComponent> _animationComponent;

public:
    AnimationStateAction(std::string animationGuid, std::string onCompleteTrigger, bool loop,
                         float animationSpeed, bool disableVelocitySpeed, bool disableMovement,
                         float disableMovementDuration,
                         std::weak_ptr<FsmAnimationComponent> animationComponent)
        : _animationGuid(std::move(animationGuid)),
          _onCompleteTrigger(std::move(onCompleteTrigger)),
          _loop(loop),
          _animationSpeed(animationSpeed),
          _disableVelocitySpeed(disableVelocitySpeed),
          _disableMovement(disableMovement),
          _disableMovementDuration(disableMovementDuration),
          _animationComponent(std::move(animationComponent)) {}

    void OnEnter() const override {
        if (const auto comp = _animationComponent.lock()) {
            // Only reset time if switching to a different animation
            // (preserve time when coming from a transition)
            if (comp->CurrentAnimationGuid != _animationGuid) {
                comp->Time = 0.0f;
                ANIM_STATE_LOG << "[ANIM STATE] Switching animation: " << comp->CurrentAnimationGuid
                               << " -> " << _animationGuid << " (loop=" << _loop << ")" << std::endl;
            } else {
                ANIM_STATE_LOG << "[ANIM STATE] Entering state with same animation: " << _animationGuid << std::endl;
            }
            comp->CurrentAnimationGuid = _animationGuid;
            comp->OnCompleteTrigger = _onCompleteTrigger;
            comp->Loop = _loop;
            comp->HasCompletedOnce = false;
            // Set per-state speed overrides
            comp->StateAnimationSpeed = _animationSpeed;
            comp->StateDisableVelocitySpeed = _disableVelocitySpeed;
            comp->MovementDisabled = _disableMovement;
            comp->MovementDisabledDuration = _disableMovementDuration;
            comp->MovementDisabledTimer = 0.0f;  // Reset timer when entering state
        }
    }

    void OnUpdate() const override {
        // Animation playback is handled by FsmAnimationSystem
    }

    void OnExit() const override {
        // Keep animation playing on exit
    }
};
