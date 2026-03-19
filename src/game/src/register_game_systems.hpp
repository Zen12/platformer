#pragma once
#include "core/src/register/esc_system_registry.hpp"
#include "esc/esc_system_context.hpp"

#include "scene.hpp"

#include "player_controller/player_controller_system.hpp"
#include "spawner/spawner_system.hpp"
#include "plugins/ai/src/combat_state/combat_state_component.hpp"
#include "health/health_system.hpp"
#include "shooting/shooting_system.hpp"
#include "navigation_manager_component.hpp"
#include "bone_attachment/bone_attachment_system.hpp"

inline void RegisterGameSystems(EscSystemRegistry& registry) {
    // PlayerControllerSystem: priority 150
    registry.Register<PlayerControllerSystem>("PlayerControllerSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<PlayerControllerSystem>(nullptr);
        const auto reg = ctx.Registry;
        return std::make_unique<PlayerControllerSystem>(
            reg->view<PlayerControllerComponent, NavmeshAgentComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(), scenePtr, *reg);
    }, 150, SystemPhase::FIXED);

    // ShootingSystem: priority 210
    registry.Register<ShootingSystem>("ShootingSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<ShootingSystem>(nullptr);
        const auto reg = ctx.Registry;
        std::shared_ptr<NavigationManager> navManager;
        for (const auto& [_, comp] : reg->view<NavigationManagerComponent>().each()) {
            navManager = comp.Manager;
            break;
        }
        if (!navManager) return std::unique_ptr<ShootingSystem>(nullptr);
        return std::make_unique<ShootingSystem>(
            reg->view<PlayerControllerComponent, TransformComponentV2, IKAimComponent, NavmeshAgentComponent>(),
            reg->view<DeltaTimeComponent>(),
            reg->view<TagComponent, ParticleEmitterComponent>(),
            scenePtr, *reg, navManager, ctx.AudioManager);
    }, 210, SystemPhase::FIXED);

    // SpawnerSystem: priority 250
    registry.Register<SpawnerSystem>("SpawnerSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<SpawnerSystem>(nullptr);
        const auto reg = ctx.Registry;
        return std::make_unique<SpawnerSystem>(reg->view<SpawnerComponent>(), scenePtr);
    }, 250, SystemPhase::FIXED);

    // BoneAttachmentSystem: priority 215 (after IK at 210, before SkinnedMeshRender at 220)
    registry.Register<BoneAttachmentSystem>("BoneAttachmentSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<BoneAttachmentSystem>(
            reg->view<BoneAttachmentComponent, TransformComponentV2>(), *reg);
    }, 215, SystemPhase::RENDER);

    // HealthSystem: priority 270
    registry.Register<HealthSystem>("HealthSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<HealthSystem>(
            reg->view<HealthComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(),
            reg->view<CombatStateComponent, TransformComponentV2>());
    }, 270, SystemPhase::FIXED);
}
