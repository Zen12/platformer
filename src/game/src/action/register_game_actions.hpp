#pragma once

#include "core/src/register/fsm_action_registry.hpp"
#include "engine_context.hpp"

#include "health_display_action.hpp"
#include "health_display_action_serialization.hpp"
#include "health_display_action_serialization_yaml.hpp"

#include "health_bar_action.hpp"
#include "health_bar_action_serialization.hpp"
#include "health_bar_action_serialization_yaml.hpp"

#include "health_check_action.hpp"
#include "health_check_action_serialization.hpp"
#include "health_check_action_serialization_yaml.hpp"

inline void RegisterGameActions(FsmActionRegistry& registry) {
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
