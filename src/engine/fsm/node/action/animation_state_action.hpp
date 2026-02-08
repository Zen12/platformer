#pragma once
#include "action.hpp"
#include "../../../esc/animation/fsm_animation_component.hpp"
#include "../../../system/guid.hpp"
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
    Guid _animationGuid;
    std::string _onCompleteTrigger;
    bool _loop;
    float _animationSpeed;
    bool _disableVelocitySpeed;
    bool _disableMovement;
    float _disableMovementDuration;
    bool _useDirectionalBlending;
    Guid _directionalWalkForwardGuid;
    Guid _directionalWalkBackGuid;
    Guid _directionalWalkLeftGuid;
    Guid _directionalWalkRightGuid;
    std::weak_ptr<FsmAnimationComponent> _animationComponent;

public:
    AnimationStateAction(Guid animationGuid, std::string onCompleteTrigger, bool loop,
                         float animationSpeed, bool disableVelocitySpeed, bool disableMovement,
                         float disableMovementDuration,
                         bool useDirectionalBlending,
                         Guid directionalWalkForwardGuid,
                         Guid directionalWalkBackGuid,
                         Guid directionalWalkLeftGuid,
                         Guid directionalWalkRightGuid,
                         std::weak_ptr<FsmAnimationComponent> animationComponent)
        : _animationGuid(std::move(animationGuid)),
          _onCompleteTrigger(std::move(onCompleteTrigger)),
          _loop(loop),
          _animationSpeed(animationSpeed),
          _disableVelocitySpeed(disableVelocitySpeed),
          _disableMovement(disableMovement),
          _disableMovementDuration(disableMovementDuration),
          _useDirectionalBlending(useDirectionalBlending),
          _directionalWalkForwardGuid(std::move(directionalWalkForwardGuid)),
          _directionalWalkBackGuid(std::move(directionalWalkBackGuid)),
          _directionalWalkLeftGuid(std::move(directionalWalkLeftGuid)),
          _directionalWalkRightGuid(std::move(directionalWalkRightGuid)),
          _animationComponent(std::move(animationComponent)) {}

    void OnEnter() const override {
        if (const auto comp = _animationComponent.lock()) {
            // Only reset time if switching to a different animation
            // (preserve time when coming from a transition)
            if (comp->CurrentAnimationGuid != _animationGuid) {
                comp->Time = 0.0f;
                ANIM_STATE_LOG << "[ANIM STATE] Switching animation: " << comp->CurrentAnimationGuid.ToString()
                               << " -> " << _animationGuid.ToString() << " (loop=" << _loop << ")" << std::endl;
            } else {
                ANIM_STATE_LOG << "[ANIM STATE] Entering state with same animation: " << _animationGuid.ToString() << std::endl;
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

            // Set directional blending if configured in this state
            if (_useDirectionalBlending) {
                comp->UseDirectionalBlending = true;
                comp->DirectionalWalkForwardGuid = _directionalWalkForwardGuid;
                comp->DirectionalWalkBackGuid = _directionalWalkBackGuid;
                comp->DirectionalWalkLeftGuid = _directionalWalkLeftGuid;
                comp->DirectionalWalkRightGuid = _directionalWalkRightGuid;
            }
        }
    }

    void OnUpdate() const override {
        // Animation playback is handled by FsmAnimationSystem
    }

    void OnExit() const override {
        // Disable directional blending when exiting state (if this state had it enabled)
        if (_useDirectionalBlending) {
            if (const auto comp = _animationComponent.lock()) {
                comp->UseDirectionalBlending = false;
            }
        }
    }
};
