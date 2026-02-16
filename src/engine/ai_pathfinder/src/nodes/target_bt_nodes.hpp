#pragma once
#include "bt_node.hpp"
#include "bt_component.hpp"
#include "../../../esc/transform/transform_component.hpp"
#include "../../../esc/tag/tag_component.hpp"
#include "../../../fsm/type_list.hpp"
#include <glm/glm.hpp>
#include <limits>

struct FindTargetByTagNode final : IBTNode {
    BTStatus Execute(BTContext& ctx) override {
        auto* transform = ctx.Registry.try_get<TransformComponentV2>(ctx.Entity);
        if (!transform) return BTStatus::Failure;

        const auto myPos = transform->GetPosition();
        float searchRadius = ctx.Def.Param1;
        float bestDist = searchRadius > 0 ? searchRadius : std::numeric_limits<float>::max();
        entt::entity bestTarget = entt::null;
        glm::vec3 bestPos{0};

        auto view = ctx.Registry.view<TagComponent, TransformComponentV2>();
        for (auto entity : view) {
            if (entity == ctx.Entity) continue;

            const auto& tagComp = view.get<TagComponent>(entity);
            if (tagComp.GetTag() != ctx.Def.StringParam) continue;

            const auto& targetTransform = view.get<TransformComponentV2>(entity);
            const auto targetPos = targetTransform.GetPosition();
            float dist = glm::distance(myPos, targetPos);

            if (dist < bestDist) {
                bestDist = dist;
                bestTarget = entity;
                bestPos = targetPos;
            }
        }

        if (bestTarget != entt::null) {
            ctx.BT.TargetEntity = bestTarget;
            ctx.BT.TargetPosition = bestPos;
            ctx.BT.HasTarget = true;
            ctx.BT.TargetTag = ctx.Def.StringParam;
            return BTStatus::Success;
        }

        return BTStatus::Failure;
    }
};

struct HasTargetInRangeNode final : IBTNode {
    BTStatus Execute(BTContext& ctx) override {
        if (!ctx.BT.HasTarget) return BTStatus::Failure;
        if (ctx.BT.TargetEntity == entt::null) return BTStatus::Failure;

        auto* transform = ctx.Registry.try_get<TransformComponentV2>(ctx.Entity);
        if (!transform) return BTStatus::Failure;

        if (!ctx.Registry.valid(ctx.BT.TargetEntity)) return BTStatus::Failure;

        auto* targetTransform = ctx.Registry.try_get<TransformComponentV2>(ctx.BT.TargetEntity);
        if (!targetTransform) return BTStatus::Failure;

        float dist = glm::distance(transform->GetPosition(), targetTransform->GetPosition());
        return (dist <= ctx.Def.Param1) ? BTStatus::Success : BTStatus::Failure;
    }
};

struct CheckDistanceNode final : IBTNode {
    BTStatus Execute(BTContext& ctx) override {
        if (!ctx.BT.HasTarget) return BTStatus::Failure;

        auto* transform = ctx.Registry.try_get<TransformComponentV2>(ctx.Entity);
        if (!transform) return BTStatus::Failure;

        float dist = glm::distance(transform->GetPosition(), ctx.BT.TargetPosition);
        return (dist <= ctx.Def.Param1) ? BTStatus::Success : BTStatus::Failure;
    }
};

using TargetBTNodeTypes = fsm::TypeList<
    FindTargetByTagNode,
    HasTargetInRangeNode,
    CheckDistanceNode
>;
