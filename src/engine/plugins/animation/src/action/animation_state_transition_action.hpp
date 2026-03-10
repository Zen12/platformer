#pragma once
#include "node/action/action.hpp"
#include "../component/fsm_animation_component.hpp"
#include "guid.hpp"
#include <memory>
#include <string>

struct AnimationStateTransitionAction final : public Action {
private:
    Guid _fromAnimationGuid;
    Guid _toAnimationGuid;
    float _transitionTime;
    std::string _onCompleteTrigger;
    std::weak_ptr<FsmAnimationComponent> _animationComponent;

public:
    AnimationStateTransitionAction(Guid fromAnimationGuid, Guid toAnimationGuid, float transitionTime, std::string onCompleteTrigger, std::weak_ptr<FsmAnimationComponent> animationComponent)
        : _fromAnimationGuid(std::move(fromAnimationGuid)),
          _toAnimationGuid(std::move(toAnimationGuid)),
          _transitionTime(transitionTime),
          _onCompleteTrigger(std::move(onCompleteTrigger)),
          _animationComponent(std::move(animationComponent)) {}

    void OnEnter() const override {
        if (const auto comp = _animationComponent.lock()) {
            // Start transition
            comp->IsTransitioning = true;
            comp->FromAnimationGuid = _fromAnimationGuid;
            comp->ToAnimationGuid = _toAnimationGuid;
            comp->TransitionDuration = _transitionTime;
            comp->TransitionProgress = 0.0f;
            comp->OnCompleteTrigger = _onCompleteTrigger;
            comp->HasCompletedOnce = false;

            // Set current animation to from animation if not already set
            if (comp->CurrentAnimationGuid != _fromAnimationGuid) {
                comp->CurrentAnimationGuid = _fromAnimationGuid;
                comp->Time = 0.0f;
            }
        }
    }

    void OnUpdate() const override {
        // Transition is handled by FsmAnimationSystem
    }

    void OnExit() const override {
        // Keep animation playing on exit
    }
};
