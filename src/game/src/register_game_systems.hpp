#pragma once
#include "core/src/register/esc_system_registry.hpp"
#include "esc/esc_system_context.hpp"

#include "scene.hpp"

#include "player_controller/player_controller_system.hpp"
#include "spawner/spawner_system.hpp"
#include "plugins/ai/src/combat_state/combat_state_component.hpp"
#include "health/health_system.hpp"

inline void RegisterGameSystems(EscSystemRegistry& registry) {
    // PlayerControllerSystem: priority 150
    registry.Register<PlayerControllerSystem>("PlayerControllerSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<PlayerControllerSystem>(nullptr);
        const auto reg = ctx.Registry;
        return std::make_unique<PlayerControllerSystem>(
            reg->view<PlayerControllerComponent, NavmeshAgentComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(), scenePtr, *reg);
    }, 150);

    // SpawnerSystem: priority 250
    registry.Register<SpawnerSystem>("SpawnerSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<SpawnerSystem>(nullptr);
        const auto reg = ctx.Registry;
        return std::make_unique<SpawnerSystem>(reg->view<SpawnerComponent>(), scenePtr);
    }, 250);

    // HealthSystem: priority 270
    registry.Register<HealthSystem>("HealthSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<HealthSystem>(
            reg->view<HealthComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(),
            reg->view<CombatStateComponent, TransformComponentV2>());
    }, 270);
}
