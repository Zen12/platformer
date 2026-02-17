#pragma once

#include "action_registry.hpp"
#include "condition_registry.hpp"
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

inline void RegisterCoreActions(ActionRegistry& registry) {
    registry.Register("load_ui_page",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<UIPageActionSerialization>();
            return std::make_unique<UIPageActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<Action> {
            const auto* s = dynamic_cast<const UIPageActionSerialization*>(ser);
            if (!s) return nullptr;
            return std::make_unique<UiPageAction>(s->UiPageGuid, ctx.Get<UIManager>("UIManager"));
        }
    );

    registry.Register("load_scene",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<LoadSceneActionSerialization>();
            return std::make_unique<LoadSceneActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<Action> {
            const auto* s = dynamic_cast<const LoadSceneActionSerialization*>(ser);
            if (!s) return nullptr;
            return std::make_unique<LoadSceneAction>(s->SceneGuid, ctx.Get<SceneManager>("SceneManager"));
        }
    );

    registry.Register("action_button_listener",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<ButtonListenerActionSerialization>();
            return std::make_unique<ButtonListenerActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<Action> {
            const auto* s = dynamic_cast<const ButtonListenerActionSerialization*>(ser);
            if (!s) return nullptr;
            return std::make_unique<ButtonListenerAction>(s->ButtonId, ctx.Get<UIManager>("UIManager"));
        }
    );

    registry.Register("action_trigger_setter_button_listener",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<TriggerSetterButtonListenerActionSerialization>();
            return std::make_unique<TriggerSetterButtonListenerActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<Action> {
            const auto* s = dynamic_cast<const TriggerSetterButtonListenerActionSerialization*>(ser);
            if (!s) return nullptr;
            return std::make_unique<TriggerSetterButtonListenerAction>(
                s->ButtonId, s->TriggerName,
                ctx.Get<UIManager>("UIManager"),
                ctx.Get<TriggerBoard>("TriggerBoard"));
        }
    );

    registry.Register("set_system_trigger",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<SetSystemTriggerActionSerialization>();
            return std::make_unique<SetSystemTriggerActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<Action> {
            const auto* s = dynamic_cast<const SetSystemTriggerActionSerialization*>(ser);
            if (!s) return nullptr;
            SystemTriggers triggerType;
            if (s->TriggerType == "Exit") {
                triggerType = SystemTriggers::Exit;
            } else if (s->TriggerType == "Reload") {
                triggerType = SystemTriggers::Reload;
            } else {
                triggerType = SystemTriggers::Exit;
            }
            return std::make_unique<SetSystemTriggerAction>(triggerType, ctx.Get<TriggerBoard>("TriggerBoard"));
        }
    );

    registry.Register("log",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<LogActionSerialization>();
            return std::make_unique<LogActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, [[maybe_unused]] const EngineContext& ctx) -> std::unique_ptr<Action> {
            const auto* s = dynamic_cast<const LogActionSerialization*>(ser);
            if (!s) return nullptr;
            return std::make_unique<LogAction>(s->Message);
        }
    );

    registry.Register("animation_state",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<AnimationStateActionSerialization>();
            return std::make_unique<AnimationStateActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<Action> {
            const auto* s = dynamic_cast<const AnimationStateActionSerialization*>(ser);
            if (!s) return nullptr;
            return std::make_unique<AnimationStateAction>(
                Guid::FromString(s->AnimationGuid),
                s->OnCompleteTrigger,
                s->Loop,
                s->AnimationSpeed,
                s->DisableVelocitySpeed,
                s->DisableMovement,
                s->DisableMovementDuration,
                s->UseDirectionalBlending,
                Guid::FromString(s->DirectionalWalkForwardGuid),
                Guid::FromString(s->DirectionalWalkBackGuid),
                Guid::FromString(s->DirectionalWalkLeftGuid),
                Guid::FromString(s->DirectionalWalkRightGuid),
                ctx.GetWeak<FsmAnimationComponent>("FsmAnimationComponent"));
        }
    );

    registry.Register("animation_state_transition",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<AnimationStateTransitionActionSerialization>();
            return std::make_unique<AnimationStateTransitionActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<Action> {
            const auto* s = dynamic_cast<const AnimationStateTransitionActionSerialization*>(ser);
            if (!s) return nullptr;
            return std::make_unique<AnimationStateTransitionAction>(
                Guid::FromString(s->FromAnimationGuid),
                Guid::FromString(s->ToAnimationGuid),
                s->TransitionTime,
                s->OnCompleteTrigger,
                ctx.GetWeak<FsmAnimationComponent>("FsmAnimationComponent"));
        }
    );

    registry.Register("start_video_recording",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<StartVideoRecordingActionSerialization>();
            return std::make_unique<StartVideoRecordingActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<Action> {
            const auto* s = dynamic_cast<const StartVideoRecordingActionSerialization*>(ser);
            if (!s) return nullptr;
            return std::make_unique<StartVideoRecordingAction>(s->OutputFile, s->Fps, ctx.Get<VideoRecorder>("VideoRecorder"));
        }
    );

    registry.Register("stop_video_recording",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<StopVideoRecordingActionSerialization>();
            return std::make_unique<StopVideoRecordingActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<Action> {
            if (!dynamic_cast<const StopVideoRecordingActionSerialization*>(ser)) return nullptr;
            return std::make_unique<StopVideoRecordingAction>(ctx.Get<VideoRecorder>("VideoRecorder"));
        }
    );

    registry.Register("fps_display",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<FpsDisplayActionSerialization>();
            return std::make_unique<FpsDisplayActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<Action> {
            const auto* s = dynamic_cast<const FpsDisplayActionSerialization*>(ser);
            if (!s) return nullptr;
            return std::make_unique<FpsDisplayAction>(s->ElementId, ctx.Get<UIManager>("UIManager"), ctx.Get<SceneManager>("SceneManager"));
        }
    );

    registry.Register("health_display",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<HealthDisplayActionSerialization>();
            return std::make_unique<HealthDisplayActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<Action> {
            const auto* s = dynamic_cast<const HealthDisplayActionSerialization*>(ser);
            if (!s) return nullptr;
            return std::make_unique<HealthDisplayAction>(s->ElementId, ctx.Get<UIManager>("UIManager"), ctx.Get<SceneManager>("SceneManager"));
        }
    );

    registry.Register("health_bar",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<HealthBarActionSerialization>();
            return std::make_unique<HealthBarActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<Action> {
            const auto* s = dynamic_cast<const HealthBarActionSerialization*>(ser);
            if (!s) return nullptr;
            return std::make_unique<HealthBarAction>(s->ElementId, ctx.Get<UIManager>("UIManager"), ctx.Get<SceneManager>("SceneManager"));
        }
    );

    registry.Register("health_check",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<HealthCheckActionSerialization>();
            return std::make_unique<HealthCheckActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<Action> {
            const auto* s = dynamic_cast<const HealthCheckActionSerialization*>(ser);
            if (!s) return nullptr;
            return std::make_unique<HealthCheckAction>(s->TriggerName, ctx.Get<SceneManager>("SceneManager"), ctx.Get<TriggerBoard>("TriggerBoard"));
        }
    );
}

inline void RegisterCoreConditions(ConditionRegistry& registry) {
    registry.Register("trigger_check",
        [](const YAML::Node& n) -> std::unique_ptr<ConditionSerialization> {
            auto s = n.as<TriggerCheckConditionSerialization>();
            return std::make_unique<TriggerCheckConditionSerialization>(std::move(s));
        },
        [](const ConditionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<ConditionBase> {
            const auto* s = dynamic_cast<const TriggerCheckConditionSerialization*>(ser);
            if (!s) return nullptr;
            auto cond = std::make_unique<TriggerCheckCondition>(s->TriggerName, ctx.Get<TriggerBoard>("TriggerBoard"));
            cond->Type = s->Type;
            cond->Guid = s->Guid;
            return cond;
        }
    );

    registry.Register("always_true",
        [](const YAML::Node& n) -> std::unique_ptr<ConditionSerialization> {
            auto s = n.as<AlwaysTrueConditionSerialization>();
            return std::make_unique<AlwaysTrueConditionSerialization>(std::move(s));
        },
        [](const ConditionSerialization* ser, [[maybe_unused]] const EngineContext& ctx) -> std::unique_ptr<ConditionBase> {
            auto cond = std::make_unique<AlwaysTrueCondition>();
            cond->Type = ser->Type;
            cond->Guid = ser->Guid;
            return cond;
        }
    );
}
