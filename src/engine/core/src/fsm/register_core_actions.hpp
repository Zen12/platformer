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

#include "node/action/fps_display_action.hpp"
#include "node/action/fps_display_action_serialization.hpp"
#include "node/action/fps_display_action_serialization_yaml.hpp"

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

    registry.Register<FpsDisplayActionSerialization>("fps_display",
        [](const FpsDisplayActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<FpsDisplayAction>(s.ElementId, ctx.Get<UIManager>("UIManager"), ctx.Get<SceneManager>("SceneManager"));
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
