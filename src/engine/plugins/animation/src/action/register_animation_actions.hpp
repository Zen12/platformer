#pragma once

#include "../../../core/src/register/fsm_action_registry.hpp"
#include "engine_context.hpp"
#include "guid.hpp"

#include "animation_state_action.hpp"
#include "animation_state_action_serialization.hpp"
#include "animation_state_action_serialization_yaml.hpp"

#include "animation_state_transition_action.hpp"
#include "animation_state_transition_action_serialization.hpp"
#include "animation_state_transition_action_serialization_yaml.hpp"

inline void RegisterAnimationActions(FsmActionRegistry& registry) {
    registry.Register<AnimationStateActionSerialization>("animation_state",
        [](const AnimationStateActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<AnimationStateAction>(
                Guid::FromString(s.AnimationGuid),
                s.OnCompleteTrigger,
                s.Loop,
                s.AnimationSpeed,
                s.DisableVelocitySpeed,
                s.DisableMovement,
                s.DisableMovementDuration,
                s.UseDirectionalBlending,
                Guid::FromString(s.DirectionalWalkForwardGuid),
                Guid::FromString(s.DirectionalWalkBackGuid),
                Guid::FromString(s.DirectionalWalkLeftGuid),
                Guid::FromString(s.DirectionalWalkRightGuid),
                ctx.GetWeak<FsmAnimationComponent>("FsmAnimationComponent"));
        }
    );

    registry.Register<AnimationStateTransitionActionSerialization>("animation_state_transition",
        [](const AnimationStateTransitionActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<AnimationStateTransitionAction>(
                Guid::FromString(s.FromAnimationGuid),
                Guid::FromString(s.ToAnimationGuid),
                s.TransitionTime,
                s.OnCompleteTrigger,
                ctx.GetWeak<FsmAnimationComponent>("FsmAnimationComponent"));
        }
    );
}
