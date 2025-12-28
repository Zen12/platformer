#pragma once
#include "action.hpp"
#include "../../../esc/animation/fsm_animation_component.hpp"
#include <memory>
#include <string>

struct AnimationStateAction final : public Action {
private:
    std::string _animationGuid;
    std::string _onCompleteTrigger;
    std::weak_ptr<FsmAnimationComponent> _animationComponent;

public:
    AnimationStateAction(std::string animationGuid, std::string onCompleteTrigger, std::weak_ptr<FsmAnimationComponent> animationComponent)
        : _animationGuid(std::move(animationGuid)),
          _onCompleteTrigger(std::move(onCompleteTrigger)),
          _animationComponent(std::move(animationComponent)) {}

    void OnEnter() const override {
        if (const auto comp = _animationComponent.lock()) {
            // Only reset time if switching to a different animation
            // (preserve time when coming from a transition)
            if (comp->CurrentAnimationGuid != _animationGuid) {
                comp->Time = 0.0f;
            }
            comp->CurrentAnimationGuid = _animationGuid;
            comp->FirstFrame = true;
            comp->OnCompleteTrigger = _onCompleteTrigger;
            comp->HasCompletedOnce = false;
        }
    }

    void OnUpdate() const override {
        // Animation playback is handled by FsmAnimationSystem
    }

    void OnExit() const override {
        // Keep animation playing on exit
    }
};
