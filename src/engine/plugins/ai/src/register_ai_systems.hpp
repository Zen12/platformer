#pragma once

#include "core/src/register/esc_system_registry.hpp"
#include "esc/esc_system_context.hpp"

#include "bt_system.hpp"
#include "esc/time/time_component.hpp"
#include "esc/camera/camera_component.hpp"
#include "esc/transform/transform_component.hpp"

#include "scene.hpp"

inline void RegisterAISystems(EscSystemRegistry& registry) {
    // BehaviorTreeSystem: priority 260
    registry.Register("BehaviorTreeSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return nullptr;
        const auto reg = ctx.Registry;
        return std::make_unique<BehaviorTreeSystem>(
            reg->view<BehaviorTreeComponent>(),
            *reg,
            scenePtr,
            reg->view<DeltaTimeComponent>(),
            reg->view<CameraComponentV2, TransformComponentV2>());
    }, 260);
}
