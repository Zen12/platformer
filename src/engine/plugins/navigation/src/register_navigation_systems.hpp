#pragma once
#include "core/src/register/esc_system_registry.hpp"
#include "esc/esc_system_context.hpp"
#include "buffer/render_buffer_component.hpp"

#include "navigation_manager_component.hpp"
#include "navmesh_agent/navmesh_agent_system.hpp"
#include "navmesh_path_renderer/navmesh_path_render_system.hpp"
#include "navmesh_debug_renderer/navmesh_debug_render_system.hpp"

inline void RegisterNavigationSystems(EscSystemRegistry& registry) {
    // NavmeshAgentSystem: priority 160
    registry.Register<NavmeshAgentSystem>("NavmeshAgentSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        std::shared_ptr<NavigationManager> navManager;
        for (const auto& [_, comp] : reg->view<NavigationManagerComponent>().each()) {
            navManager = comp.Manager;
            break;
        }
        if (!navManager) return std::unique_ptr<NavmeshAgentSystem>(nullptr);
        return std::make_unique<NavmeshAgentSystem>(
            reg->view<NavmeshAgentComponent, TransformComponentV2>(),
            reg->view<DeltaTimeComponent>(), navManager);
    }, 160, SystemPhase::FIXED);

#ifndef NDEBUG
    // NavmeshPathRenderSystem: priority 230
    registry.Register<NavmeshPathRenderSystem>("NavmeshPathRenderSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        std::shared_ptr<NavigationManager> navManager;
        for (const auto& [_, comp] : reg->view<NavigationManagerComponent>().each()) {
            navManager = comp.Manager;
            break;
        }
        if (!navManager) return std::unique_ptr<NavmeshPathRenderSystem>(nullptr);
        return std::make_unique<NavmeshPathRenderSystem>(
            reg->view<NavmeshAgentComponent>(),
            reg->view<CameraComponentV2>(), reg->view<RenderBufferComponent>(), navManager);
    }, 230, SystemPhase::RENDER);

    // NavmeshDebugRenderSystem: priority 240
    registry.Register<NavmeshDebugRenderSystem>("NavmeshDebugRenderSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        std::shared_ptr<NavigationManager> navManager;
        for (const auto& [_, comp] : reg->view<NavigationManagerComponent>().each()) {
            navManager = comp.Manager;
            break;
        }
        if (!navManager) return std::unique_ptr<NavmeshDebugRenderSystem>(nullptr);
        return std::make_unique<NavmeshDebugRenderSystem>(
            reg->view<CameraComponentV2>(), reg->view<RenderBufferComponent>(), navManager);
    }, 240, SystemPhase::RENDER);
#endif
}
