#pragma once

#include "../register/fsm_action_registry.hpp"
#include "../register/fsm_condition_registry.hpp"
#include "engine_context.hpp"
#include "trigger_board.hpp"
#include "guid.hpp"

// Action headers
#include "ui/ui_page_action.hpp"
#include "ui/ui_page_action_serialization.hpp"
#include "ui/ui_page_action_serialization_yaml.hpp"

#include "load_scene_action.hpp"
#include "load_scene_action_serialization.hpp"
#include "load_scene_action_serialization_yaml.hpp"

#include "ui/button_listener_action.hpp"
#include "ui/button_listener_action_serialization.hpp"
#include "ui/button_listener_action_serialization_yaml.hpp"

#include "ui/trigger_setter_button_listener_action.hpp"
#include "ui/trigger_setter_button_listener_action_serialization.hpp"
#include "ui/trigger_setter_button_listener_action_serialization_yaml.hpp"

#include "node/action/set_system_trigger_action.hpp"
#include "node/action/set_system_trigger_action_serialization.hpp"
#include "node/action/set_system_trigger_action_serialization_yaml.hpp"

#include "node/action/log_action.hpp"
#include "node/action/log_action_serialization.hpp"
#include "node/action/log_action_serialization_yaml.hpp"

#include "node/action/animation_state_action.hpp"
#include "node/action/animation_state_action_serialization.hpp"
#include "node/action/animation_state_action_serialization_yaml.hpp"

#include "node/action/animation_state_transition_action.hpp"
#include "node/action/animation_state_transition_action_serialization.hpp"
#include "node/action/animation_state_transition_action_serialization_yaml.hpp"

#include "node/action/start_video_recording_action.hpp"
#include "node/action/start_video_recording_action_serialization.hpp"
#include "node/action/start_video_recording_action_serialization_yaml.hpp"

#include "node/action/stop_video_recording_action.hpp"
#include "node/action/stop_video_recording_action_serialization.hpp"
#include "node/action/stop_video_recording_action_serialization_yaml.hpp"

#include "node/action/fps_display_action.hpp"
#include "node/action/fps_display_action_serialization.hpp"
#include "node/action/fps_display_action_serialization_yaml.hpp"

#include "node/action/health_display_action.hpp"
#include "node/action/health_display_action_serialization.hpp"
#include "node/action/health_display_action_serialization_yaml.hpp"

#include "node/action/health_bar_action.hpp"
#include "node/action/health_bar_action_serialization.hpp"
#include "node/action/health_bar_action_serialization_yaml.hpp"

#include "node/action/health_check_action.hpp"
#include "node/action/health_check_action_serialization.hpp"
#include "node/action/health_check_action_serialization_yaml.hpp"

// Condition headers
#include "condition/core_types/trigger_check_condition.hpp"
#include "condition/core_types/trigger_check_condition_serialization.hpp"
#include "condition/core_types/trigger_check_condition_serialization_yaml.hpp"

#include "condition/core_types/always_true_condition.hpp"
#include "condition/core_types/always_true_condition_serialization.hpp"
#include "condition/core_types/always_true_condition_serialization_yaml.hpp"

inline void RegisterCoreActions(FsmActionRegistry& registry) {
    registry.Register<UIPageActionSerialization>("load_ui_page",
        [](const UIPageActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<UiPageAction>(s.UiPageGuid, ctx.Get<UIManager>("UIManager"));
        }
    );

    registry.Register<LoadSceneActionSerialization>("load_scene",
        [](const LoadSceneActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<LoadSceneAction>(s.SceneGuid, ctx.Get<SceneManager>("SceneManager"));
        }
    );

    registry.Register<ButtonListenerActionSerialization>("action_button_listener",
        [](const ButtonListenerActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<ButtonListenerAction>(s.ButtonId, ctx.Get<UIManager>("UIManager"));
        }
    );

    registry.Register<TriggerSetterButtonListenerActionSerialization>("action_trigger_setter_button_listener",
        [](const TriggerSetterButtonListenerActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<TriggerSetterButtonListenerAction>(
                s.ButtonId, s.TriggerName,
                ctx.Get<UIManager>("UIManager"),
                ctx.Get<TriggerBoard>("TriggerBoard"));
        }
    );

    registry.Register<SetSystemTriggerActionSerialization>("set_system_trigger",
        [](const SetSystemTriggerActionSerialization& s, const EngineContext& ctx) {
            SystemTriggers triggerType;
            if (s.TriggerType == "Exit") {
                triggerType = SystemTriggers::Exit;
            } else if (s.TriggerType == "Reload") {
                triggerType = SystemTriggers::Reload;
            } else {
                triggerType = SystemTriggers::Exit;
            }
            return std::make_unique<SetSystemTriggerAction>(triggerType, ctx.Get<TriggerBoard>("TriggerBoard"));
        }
    );

    registry.Register<LogActionSerialization>("log",
        [](const LogActionSerialization& s, [[maybe_unused]] const EngineContext& ctx) {
            return std::make_unique<LogAction>(s.Message);
        }
    );

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

    registry.Register<StartVideoRecordingActionSerialization>("start_video_recording",
        [](const StartVideoRecordingActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<StartVideoRecordingAction>(s.OutputFile, s.Fps, ctx.Get<VideoRecorder>("VideoRecorder"));
        }
    );

    registry.Register<StopVideoRecordingActionSerialization>("stop_video_recording",
        []([[maybe_unused]] const StopVideoRecordingActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<StopVideoRecordingAction>(ctx.Get<VideoRecorder>("VideoRecorder"));
        }
    );

    registry.Register<FpsDisplayActionSerialization>("fps_display",
        [](const FpsDisplayActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<FpsDisplayAction>(s.ElementId, ctx.Get<UIManager>("UIManager"), ctx.Get<SceneManager>("SceneManager"));
        }
    );

    registry.Register<HealthDisplayActionSerialization>("health_display",
        [](const HealthDisplayActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<HealthDisplayAction>(s.ElementId, ctx.Get<UIManager>("UIManager"), ctx.Get<SceneManager>("SceneManager"));
        }
    );

    registry.Register<HealthBarActionSerialization>("health_bar",
        [](const HealthBarActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<HealthBarAction>(s.ElementId, ctx.Get<UIManager>("UIManager"), ctx.Get<SceneManager>("SceneManager"));
        }
    );

    registry.Register<HealthCheckActionSerialization>("health_check",
        [](const HealthCheckActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<HealthCheckAction>(s.TriggerName, ctx.Get<SceneManager>("SceneManager"), ctx.Get<TriggerBoard>("TriggerBoard"));
        }
    );
}

inline void RegisterCoreConditions(FsmConditionRegistry& registry) {
    registry.Register<TriggerCheckConditionSerialization>("trigger_check",
        [](const TriggerCheckConditionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<TriggerCheckCondition>(s.TriggerName, ctx.Get<TriggerBoard>("TriggerBoard"));
        }
    );

    registry.Register<AlwaysTrueConditionSerialization>("always_true",
        []([[maybe_unused]] const AlwaysTrueConditionSerialization& s, [[maybe_unused]] const EngineContext& ctx) {
            return std::make_unique<AlwaysTrueCondition>();
        }
    );
}
