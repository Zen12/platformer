#pragma once
#include "core/src/register/esc_system_registry.hpp"
#include "esc_system_context.hpp"

#include "scene.hpp"
#include "resource_cache.hpp"
#include "render_repository.hpp"
#include "audio_manager.hpp"

#include "window/window_system.hpp"
#include "time/time_component.hpp"
#include "camera/camera_controller_system.hpp"
#include "camera/camera_system.hpp"
#include "directional_light/directional_light_system.hpp"
#include "player_controller/player_controller_system.hpp"
#include "navmesh_agent/navmesh_agent_system.hpp"
#include "camera/top_down_camera_system.hpp"
#include "mesh_renderer/mesh_render_system.hpp"
#include "animation/simple_animation_system.hpp"
#include "animation/fsm_animation_system.hpp"
#include "ik_aim/ik_aim_system.hpp"
#include "skinned_mesh_renderer/skinned_mesh_render_system.hpp"
#include "navmesh_path_renderer/navmesh_path_render_system.hpp"
#include "navmesh_debug_renderer/navmesh_debug_render_system.hpp"
#include "spawner/spawner_system.hpp"
#include "combat_state/combat_state_component.hpp"
#include "health/health_system.hpp"
#include "audio_source/audio_system.hpp"
#include "particle_emitter/particle_system.hpp"
#include "particle_emitter/particle_render_system.hpp"

inline void RegisterCoreSystems(EscSystemRegistry& registry) {
    // WindowSystem: priority 100
    registry.Register("WindowSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return nullptr;
        const auto reg = ctx.Registry;
        const auto& windowView = reg->view<WindowComponent>();
        return std::make_unique<WindowSystem>(windowView, scenePtr->GetWindow());
    }, 100);

    // DeltaTimeSystem: priority 110
    registry.Register("DeltaTimeSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto reg = ctx.Registry;
        return std::make_unique<DeltaTimeSystem>(reg->view<DeltaTimeComponent>());
    }, 110);

    // CameraControllerSystem: priority 120
    registry.Register("CameraControllerSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return nullptr;
        const auto reg = ctx.Registry;
        return std::make_unique<CameraControllerSystem>(
            reg->view<CameraControllerComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(), scenePtr);
    }, 120);

    // CameraSystem: priority 130
    registry.Register("CameraSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto reg = ctx.Registry;
        return std::make_unique<CameraSystem>(
            reg->view<WindowComponent>(),
            reg->view<CameraComponentV2, TransformComponentV2>());
    }, 130);

    // DirectionalLightSystem: priority 140
    registry.Register("DirectionalLightSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto reg = ctx.Registry;
        return std::make_unique<DirectionalLightSystem>(
            reg->view<DirectionalLightComponent, TransformComponentV2>(),
            reg->view<TagComponent, TransformComponentV2>(),
            ctx.RenderRepository);
    }, 140);

    // PlayerControllerSystem: priority 150
    registry.Register("PlayerControllerSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return nullptr;
        const auto reg = ctx.Registry;
        return std::make_unique<PlayerControllerSystem>(
            reg->view<PlayerControllerComponent, NavmeshAgentComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(), scenePtr, *reg);
    }, 150);

    // NavmeshAgentSystem: priority 160
    registry.Register("NavmeshAgentSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return nullptr;
        const auto reg = ctx.Registry;
        return std::make_unique<NavmeshAgentSystem>(
            reg->view<NavmeshAgentComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(), scenePtr->GetNavigationManager(), scenePtr);
    }, 160);

    // TopDownCameraSystem: priority 170
    registry.Register("TopDownCameraSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto reg = ctx.Registry;
        return std::make_unique<TopDownCameraSystem>(
            reg->view<TopDownCameraComponent, TransformComponentV2>(),
            reg->view<TagComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(),
            *reg);
    }, 170);

    // MeshRenderSystem: priority 180
    registry.Register("MeshRenderSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto reg = ctx.Registry;
        return std::make_unique<MeshRenderSystem>(
            reg->view<MeshRendererComponent, TransformComponentV2>(),
            reg->view<CameraComponentV2>(), ctx.RenderRepository, ctx.ResourceCache);
    }, 180);

    // SimpleAnimationSystem: priority 190
    registry.Register("SimpleAnimationSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto reg = ctx.Registry;
        return std::make_unique<SimpleAnimationSystem>(
            reg->view<SimpleAnimationComponent, SkinnedMeshRendererComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(), ctx.ResourceCache);
    }, 190);

    // FsmAnimationSystem: priority 200
    registry.Register("FsmAnimationSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return nullptr;
        const auto reg = ctx.Registry;
        return std::make_unique<FsmAnimationSystem>(
            reg->view<FsmAnimationComponent, SkinnedMeshRendererComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(), scenePtr, ctx.ResourceCache, *reg);
    }, 200);

    // IKAimSystem: priority 210
    registry.Register("IKAimSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return nullptr;
        const auto reg = ctx.Registry;
        return std::make_unique<IKAimSystem>(
            reg->view<IKAimComponent, SkinnedMeshRendererComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(),
            reg->view<CameraComponentV2, TransformComponentV2>(),
            reg->view<WindowComponent>(),
            scenePtr,
            *reg);
    }, 210);

    // SkinnedMeshRenderSystem: priority 220
    registry.Register("SkinnedMeshRenderSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto reg = ctx.Registry;
        return std::make_unique<SkinnedMeshRenderSystem>(
            reg->view<SkinnedMeshRendererComponent, TransformComponentV2>(),
            reg->view<CameraComponentV2>(), ctx.RenderRepository, ctx.ResourceCache);
    }, 220);

#ifndef NDEBUG
    // NavmeshPathRenderSystem: priority 230
    registry.Register("NavmeshPathRenderSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return nullptr;
        const auto reg = ctx.Registry;
        return std::make_unique<NavmeshPathRenderSystem>(
            reg->view<NavmeshAgentComponent>(),
            reg->view<CameraComponentV2>(), ctx.RenderRepository, scenePtr->GetNavigationManager());
    }, 230);

    // NavmeshDebugRenderSystem: priority 240
    registry.Register("NavmeshDebugRenderSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return nullptr;
        const auto reg = ctx.Registry;
        return std::make_unique<NavmeshDebugRenderSystem>(
            reg->view<CameraComponentV2>(), ctx.RenderRepository, scenePtr->GetNavigationManager());
    }, 240);
#endif

    // SpawnerSystem: priority 250
    registry.Register("SpawnerSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return nullptr;
        const auto reg = ctx.Registry;
        return std::make_unique<SpawnerSystem>(reg->view<SpawnerComponent>(), scenePtr);
    }, 250);

    // HealthSystem: priority 270
    registry.Register("HealthSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto reg = ctx.Registry;
        return std::make_unique<HealthSystem>(
            reg->view<HealthComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(),
            reg->view<CombatStateComponent, TransformComponentV2>());
    }, 270);

    // AudioSystem: priority 280
    registry.Register("AudioSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto reg = ctx.Registry;
        return std::make_unique<AudioSystem>(
            reg->view<AudioSourceComponent, TransformComponentV2>(),
            reg->view<AudioListenerComponent, TransformComponentV2>(),
            ctx.AudioManager);
    }, 280);

    // ParticleSystem: priority 290
    registry.Register("ParticleSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto reg = ctx.Registry;
        return std::make_unique<ParticleSystem>(
            reg->view<ParticleEmitterComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(),
            reg->view<ParticleEmitterComponent, NavmeshAgentComponent>(),
            reg->view<ParticleEmitterComponent, HealthComponent>(),
            reg->view<ParticleEmitterComponent, CombatStateComponent>());
    }, 290);

    // ParticleRenderSystem: priority 300
    registry.Register("ParticleRenderSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return nullptr;
        const auto reg = ctx.Registry;
        return std::make_unique<ParticleRenderSystem>(
            reg->view<ParticleEmitterComponent, TransformComponentV2>(),
            reg->view<CameraComponentV2>(),
            ctx.RenderRepository,
            scenePtr);
    }, 300);
}
