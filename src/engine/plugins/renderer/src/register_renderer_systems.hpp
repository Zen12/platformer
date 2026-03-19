#pragma once
#include "core/src/register/esc_system_registry.hpp"
#include "esc/esc_system_context.hpp"

#include "scene.hpp"
#include "resource_factory.hpp"
#include "buffer/render_buffer_component.hpp"
#include "window/window_system.hpp"
#include "time/time_component.hpp"
#include "camera/camera_component.hpp"
#include "camera/camera_controller_system.hpp"
#include "camera/camera_system.hpp"
#include "directional_light/directional_light_system.hpp"
#include "spot_light/spot_light_system.hpp"
#include "camera/top_down_camera_system.hpp"
#include "mesh_renderer/mesh_render_system.hpp"
#include "skinned_mesh_renderer/skinned_mesh_render_system.hpp"
#include "particle_emitter/particle_system.hpp"
#include "particle_emitter/particle_render_system.hpp"

inline void RegisterRendererSystems(EscSystemRegistry& registry) {
    // WindowSystem: priority 100
    registry.Register<WindowSystem>("WindowSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<WindowSystem>(nullptr);
        const auto reg = ctx.Registry;
        const auto& windowView = reg->view<WindowComponent>();
        return std::make_unique<WindowSystem>(windowView, scenePtr->GetWindow());
    }, 100, SystemPhase::RENDER);

    // DeltaTimeSystem: priority 110
    registry.Register<DeltaTimeSystem>("DeltaTimeSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<DeltaTimeSystem>(reg->view<DeltaTimeComponent>());
    }, 110, SystemPhase::RENDER);

    // CameraControllerSystem: priority 120
    registry.Register<CameraControllerSystem>("CameraControllerSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<CameraControllerSystem>(nullptr);
        const auto reg = ctx.Registry;
        return std::make_unique<CameraControllerSystem>(
            reg->view<CameraControllerComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(), scenePtr);
    }, 120, SystemPhase::RENDER);

    // CameraSystem: priority 130
    registry.Register<CameraSystem>("CameraSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<CameraSystem>(
            reg->view<WindowComponent>(),
            reg->view<CameraComponentV2, TransformComponentV2>());
    }, 130, SystemPhase::RENDER);

    // DirectionalLightSystem: priority 140
    registry.Register<DirectionalLightSystem>("DirectionalLightSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<DirectionalLightSystem>(
            reg->view<DirectionalLightComponent, TransformComponentV2>(),
            reg->view<TagComponent, TransformComponentV2>(),
            reg->view<RenderBufferComponent>());
    }, 140, SystemPhase::RENDER);

    // SpotLightSystem: priority 145
    registry.Register<SpotLightSystem>("SpotLightSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<SpotLightSystem>(
            reg->view<SpotLightComponent, TransformComponentV2>(),
            reg->view<CameraComponentV2>(),
            reg->view<RenderBufferComponent>());
    }, 145, SystemPhase::RENDER);

    // TopDownCameraSystem: priority 170
    registry.Register<TopDownCameraSystem>("TopDownCameraSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<TopDownCameraSystem>(
            reg->view<TopDownCameraComponent, TransformComponentV2>(),
            reg->view<TagComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(),
            *reg);
    }, 170, SystemPhase::RENDER);

    // MeshRenderSystem: priority 216 (after BoneAttachment at 215, before SkinnedMeshRender at 220)
    registry.Register<MeshRenderSystem>("MeshRenderSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<MeshRenderSystem>(
            reg->view<MeshRendererComponent, TransformComponentV2>(),
            reg->view<CameraComponentV2>(), reg->view<RenderBufferComponent>(), ctx.ResourceFactory);
    }, 216, SystemPhase::RENDER);

    // SkinnedMeshRenderSystem: priority 220
    registry.Register<SkinnedMeshRenderSystem>("SkinnedMeshRenderSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<SkinnedMeshRenderSystem>(
            reg->view<SkinnedMeshRendererComponent, TransformComponentV2>(),
            reg->view<CameraComponentV2>(), reg->view<RenderBufferComponent>(), ctx.ResourceFactory);
    }, 220, SystemPhase::RENDER);

    // ParticleSystem: priority 290
    registry.Register<ParticleSystem>("ParticleSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<ParticleSystem>(
            reg->view<ParticleEmitterComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(),
            reg->view<ParticleEmitterComponent, NavmeshAgentComponent>(),
            reg->view<ParticleEmitterComponent, HealthComponent>(),
            reg->view<ParticleEmitterComponent, CombatStateComponent>());
    }, 290, SystemPhase::RENDER);

    // ParticleRenderSystem: priority 300
    registry.Register<ParticleRenderSystem>("ParticleRenderSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<ParticleRenderSystem>(nullptr);
        const auto reg = ctx.Registry;
        return std::make_unique<ParticleRenderSystem>(
            reg->view<ParticleEmitterComponent, TransformComponentV2>(),
            reg->view<CameraComponentV2>(),
            reg->view<RenderBufferComponent>(),
            scenePtr);
    }, 300, SystemPhase::RENDER);
}
