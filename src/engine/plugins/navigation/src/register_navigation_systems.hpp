#pragma once
#include "core/src/register/esc_system_registry.hpp"
#include "esc/esc_system_context.hpp"

#include "scene.hpp"
#include "navmesh_agent/navmesh_agent_system.hpp"
#include "navmesh_path_renderer/navmesh_path_render_system.hpp"
#include "navmesh_debug_renderer/navmesh_debug_render_system.hpp"

inline void RegisterNavigationSystems(EscSystemRegistry& registry) {
    // NavmeshAgentSystem: priority 160
    registry.Register<NavmeshAgentSystem>("NavmeshAgentSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<NavmeshAgentSystem>(nullptr);
        const auto reg = ctx.Registry;
        return std::make_unique<NavmeshAgentSystem>(
            reg->view<NavmeshAgentComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(), scenePtr->GetNavigationManager(), scenePtr);
    }, 160);

#ifndef NDEBUG
    // NavmeshPathRenderSystem: priority 230
    registry.Register<NavmeshPathRenderSystem>("NavmeshPathRenderSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<NavmeshPathRenderSystem>(nullptr);
        const auto reg = ctx.Registry;
        return std::make_unique<NavmeshPathRenderSystem>(
            reg->view<NavmeshAgentComponent>(),
            reg->view<CameraComponentV2>(), ctx.RenderBuffer, scenePtr->GetNavigationManager());
    }, 230);

    // NavmeshDebugRenderSystem: priority 240
    registry.Register<NavmeshDebugRenderSystem>("NavmeshDebugRenderSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<NavmeshDebugRenderSystem>(nullptr);
        const auto reg = ctx.Registry;
        return std::make_unique<NavmeshDebugRenderSystem>(
            reg->view<CameraComponentV2>(), ctx.RenderBuffer, scenePtr->GetNavigationManager());
    }, 240);
#endif
}
