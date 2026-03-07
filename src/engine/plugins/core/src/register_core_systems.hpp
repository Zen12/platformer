#pragma once
#include "core/src/register/esc_system_registry.hpp"
#include "esc/esc_system_context.hpp"

#include "scene.hpp"
#include "resource_cache.hpp"
#include "render_repository.hpp"
#include "window/window_system.hpp"
#include "time/time_component.hpp"
#include "camera/camera_controller_system.hpp"
#include "camera/camera_system.hpp"
#include "directional_light/directional_light_system.hpp"
#include "player_controller/player_controller_system.hpp"
#include "navmesh_agent/navmesh_agent_system.hpp"
#include "camera/top_down_camera_system.hpp"
#include "mesh_renderer/mesh_render_system.hpp"
#include "skinned_mesh_renderer/skinned_mesh_render_system.hpp"
#include "navmesh_path_renderer/navmesh_path_render_system.hpp"
#include "navmesh_debug_renderer/navmesh_debug_render_system.hpp"
#include "spawner/spawner_system.hpp"
#include "plugins/ai/src/combat_state/combat_state_component.hpp"
#include "health/health_system.hpp"
#include "particle_emitter/particle_system.hpp"
#include "particle_emitter/particle_render_system.hpp"

inline void RegisterCoreSystems(EscSystemRegistry& registry) {
    // WindowSystem: priority 100
    registry.Register<WindowSystem>("WindowSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<WindowSystem>(nullptr);
        const auto reg = ctx.Registry;
        const auto& windowView = reg->view<WindowComponent>();
        return std::make_unique<WindowSystem>(windowView, scenePtr->GetWindow());
    }, 100);

    // DeltaTimeSystem: priority 110
    registry.Register<DeltaTimeSystem>("DeltaTimeSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<DeltaTimeSystem>(reg->view<DeltaTimeComponent>());
    }, 110);

    // CameraControllerSystem: priority 120
    registry.Register<CameraControllerSystem>("CameraControllerSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<CameraControllerSystem>(nullptr);
        const auto reg = ctx.Registry;
        return std::make_unique<CameraControllerSystem>(
            reg->view<CameraControllerComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(), scenePtr);
    }, 120);

    // CameraSystem: priority 130
    registry.Register<CameraSystem>("CameraSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<CameraSystem>(
            reg->view<WindowComponent>(),
            reg->view<CameraComponentV2, TransformComponentV2>());
    }, 130);

    // DirectionalLightSystem: priority 140
    registry.Register<DirectionalLightSystem>("DirectionalLightSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<DirectionalLightSystem>(
            reg->view<DirectionalLightComponent, TransformComponentV2>(),
            reg->view<TagComponent, TransformComponentV2>(),
            ctx.RenderRepository);
    }, 140);

    // PlayerControllerSystem: priority 150
    registry.Register<PlayerControllerSystem>("PlayerControllerSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<PlayerControllerSystem>(nullptr);
        const auto reg = ctx.Registry;
        return std::make_unique<PlayerControllerSystem>(
            reg->view<PlayerControllerComponent, NavmeshAgentComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(), scenePtr, *reg);
    }, 150);

    // NavmeshAgentSystem: priority 160
    registry.Register<NavmeshAgentSystem>("NavmeshAgentSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<NavmeshAgentSystem>(nullptr);
        const auto reg = ctx.Registry;
        return std::make_unique<NavmeshAgentSystem>(
            reg->view<NavmeshAgentComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(), scenePtr->GetNavigationManager(), scenePtr);
    }, 160);

    // TopDownCameraSystem: priority 170
    registry.Register<TopDownCameraSystem>("TopDownCameraSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<TopDownCameraSystem>(
            reg->view<TopDownCameraComponent, TransformComponentV2>(),
            reg->view<TagComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(),
            *reg);
    }, 170);

    // MeshRenderSystem: priority 180
    registry.Register<MeshRenderSystem>("MeshRenderSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<MeshRenderSystem>(
            reg->view<MeshRendererComponent, TransformComponentV2>(),
            reg->view<CameraComponentV2>(), ctx.RenderRepository, ctx.ResourceCache);
    }, 180);

    // SkinnedMeshRenderSystem: priority 220
    registry.Register<SkinnedMeshRenderSystem>("SkinnedMeshRenderSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<SkinnedMeshRenderSystem>(
            reg->view<SkinnedMeshRendererComponent, TransformComponentV2>(),
            reg->view<CameraComponentV2>(), ctx.RenderRepository, ctx.ResourceCache);
    }, 220);

#ifndef NDEBUG
    // NavmeshPathRenderSystem: priority 230
    registry.Register<NavmeshPathRenderSystem>("NavmeshPathRenderSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<NavmeshPathRenderSystem>(nullptr);
        const auto reg = ctx.Registry;
        return std::make_unique<NavmeshPathRenderSystem>(
            reg->view<NavmeshAgentComponent>(),
            reg->view<CameraComponentV2>(), ctx.RenderRepository, scenePtr->GetNavigationManager());
    }, 230);

    // NavmeshDebugRenderSystem: priority 240
    registry.Register<NavmeshDebugRenderSystem>("NavmeshDebugRenderSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<NavmeshDebugRenderSystem>(nullptr);
        const auto reg = ctx.Registry;
        return std::make_unique<NavmeshDebugRenderSystem>(
            reg->view<CameraComponentV2>(), ctx.RenderRepository, scenePtr->GetNavigationManager());
    }, 240);
#endif

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

    // ParticleSystem: priority 290
    registry.Register<ParticleSystem>("ParticleSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<ParticleSystem>(
            reg->view<ParticleEmitterComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(),
            reg->view<ParticleEmitterComponent, NavmeshAgentComponent>(),
            reg->view<ParticleEmitterComponent, HealthComponent>(),
            reg->view<ParticleEmitterComponent, CombatStateComponent>());
    }, 290);

    // ParticleRenderSystem: priority 300
    registry.Register<ParticleRenderSystem>("ParticleRenderSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<ParticleRenderSystem>(nullptr);
        const auto reg = ctx.Registry;
        return std::make_unique<ParticleRenderSystem>(
            reg->view<ParticleEmitterComponent, TransformComponentV2>(),
            reg->view<CameraComponentV2>(),
            ctx.RenderRepository,
            scenePtr);
    }, 300);
}
