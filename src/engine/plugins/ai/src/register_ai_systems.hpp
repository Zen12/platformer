#pragma once

#include "core/src/register/esc_system_registry.hpp"
#include "esc/esc_system_context.hpp"

#include "bt_system.hpp"
#include "plugins/core/src/time/time_component.hpp"
#include "plugins/core/src/camera/camera_component.hpp"
#include "plugins/core/src/transform/transform_component.hpp"

#include "scene.hpp"

inline void RegisterAISystems(EscSystemRegistry& registry) {
    // BehaviorTreeSystem: priority 260
    registry.Register<BehaviorTreeSystem>("BehaviorTreeSystem", [](const EscSystemContext& ctx) {
        const auto scenePtr = ctx.Scene.lock();
        if (!scenePtr) return std::unique_ptr<BehaviorTreeSystem>(nullptr);
        const auto reg = ctx.Registry;
        return std::make_unique<BehaviorTreeSystem>(
            reg->view<BehaviorTreeComponent>(),
            *reg,
            scenePtr,
            reg->view<DeltaTimeComponent>(),
            reg->view<CameraComponentV2, TransformComponentV2>());
    }, 260);
}
