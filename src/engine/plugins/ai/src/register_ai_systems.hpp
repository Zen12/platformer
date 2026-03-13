#pragma once

#include "core/src/register/esc_system_registry.hpp"
#include "esc/esc_system_context.hpp"

#include "bt_system.hpp"
#include "time/time_component.hpp"
#include "camera/camera_component.hpp"
#include "transform/transform_component.hpp"

inline void RegisterAISystems(EscSystemRegistry& registry) {
    // BehaviorTreeSystem: priority 260
    registry.Register<BehaviorTreeSystem>("BehaviorTreeSystem", [](const EscSystemContext& ctx) {
        const auto reg = ctx.Registry;
        return std::make_unique<BehaviorTreeSystem>(
            reg->view<BehaviorTreeComponent>(),
            *reg,
            ctx.ResourceFactory,
            reg->view<DeltaTimeComponent>(),
            reg->view<CameraComponentV2, TransformComponentV2>());
    }, 260);
}
