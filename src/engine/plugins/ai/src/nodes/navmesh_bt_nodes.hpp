#pragma once
#include "bt_node.hpp"
#include "bt_component.hpp"
#include "plugins/core/src/navmesh_agent/navmesh_agent_component.hpp"
#include "plugins/core/src/transform/transform_component.hpp"
#include "grid_navmesh.hpp"
#include "type_list.hpp"
#include <glm/glm.hpp>
#include <random>

struct IsMovingNode final : IBTNode {
    BTStatus Execute(BTContext& ctx) override {
        auto* navAgent = ctx.Registry.try_get<NavmeshAgentComponent>(ctx.Entity);
        if (!navAgent) return BTStatus::Failure;
        return (navAgent->CurrentSpeed > 0.1f) ? BTStatus::Success : BTStatus::Failure;
    }
};

struct MoveToTargetNode final : IBTNode {
    BTStatus Execute(BTContext& ctx) override {
        auto* navAgent = ctx.Registry.try_get<NavmeshAgentComponent>(ctx.Entity);
        auto* transform = ctx.Registry.try_get<TransformComponentV2>(ctx.Entity);
        if (!ctx.BT.HasTarget || !navAgent || !transform) return BTStatus::Failure;

        if (ctx.BT.TargetEntity != entt::null && ctx.Registry.valid(ctx.BT.TargetEntity)) {
            auto* targetTransform = ctx.Registry.try_get<TransformComponentV2>(ctx.BT.TargetEntity);
            if (targetTransform) {
                ctx.BT.TargetPosition = targetTransform->GetPosition();
            }
        }

        if (ctx.Navmesh) {
            glm::vec3 currentPos = transform->GetPosition();
            glm::vec3 targetPos = ctx.BT.TargetPosition;

            if (!ctx.Navmesh->IsWalkable(targetPos)) {
                targetPos = ctx.Navmesh->FindNearestPoint(targetPos);
            }

            auto path = ctx.Navmesh->FindPath(currentPos, targetPos);
            if (path.empty()) {
                navAgent->HasDestination = false;
                navAgent->DestinationChanged = true;
                return BTStatus::Failure;
            }

            ctx.BT.TargetPosition = targetPos;
        }

        navAgent->Destination = ctx.BT.TargetPosition;
        navAgent->HasDestination = true;
        navAgent->DestinationChanged = true;

        float distToTarget = glm::distance(transform->GetPosition(), ctx.BT.TargetPosition);
        constexpr float arrivalThreshold = 0.5f;
        if (distToTarget <= arrivalThreshold) {
            return BTStatus::Success;
        }

        return BTStatus::Running;
    }
};

struct RandomWanderNode final : IBTNode {
    BTStatus Execute(BTContext& ctx) override {
        auto* navAgent = ctx.Registry.try_get<NavmeshAgentComponent>(ctx.Entity);
        auto* transform = ctx.Registry.try_get<TransformComponentV2>(ctx.Entity);
        if (!navAgent || !transform) return BTStatus::Failure;

        if (!navAgent->HasDestination || navAgent->CurrentSpeed < 0.1f) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            float radius = ctx.Def.Param1;
            std::uniform_real_distribution<float> dist(-radius, radius);

            glm::vec3 currentPos = transform->GetPosition();
            glm::vec3 targetPos;
            bool foundValidPath = false;

            for (int attempt = 0; attempt < 5; ++attempt) {
                targetPos = currentPos + glm::vec3(dist(gen), 0, dist(gen));

                if (ctx.Navmesh) {
                    if (!ctx.Navmesh->IsWalkable(targetPos)) {
                        targetPos = ctx.Navmesh->FindNearestPoint(targetPos);
                    }
                    auto path = ctx.Navmesh->FindPath(currentPos, targetPos);
                    if (!path.empty()) {
                        foundValidPath = true;
                        break;
                    }
                } else {
                    foundValidPath = true;
                    break;
                }
            }

            if (!foundValidPath) {
                return BTStatus::Failure;
            }

            ctx.BT.TargetPosition = targetPos;
            ctx.BT.HasTarget = true;

            navAgent->Destination = ctx.BT.TargetPosition;
            navAgent->HasDestination = true;
            navAgent->DestinationChanged = true;
        }

        return BTStatus::Running;
    }
};

struct IdleNode final : IBTNode {
    BTStatus Execute(BTContext& ctx) override {
        auto* navAgent = ctx.Registry.try_get<NavmeshAgentComponent>(ctx.Entity);
        auto* transform = ctx.Registry.try_get<TransformComponentV2>(ctx.Entity);
        if (!navAgent || !transform) return BTStatus::Failure;

        float radius = ctx.Def.Param1;
        if (radius <= 0) radius = 3.0f;

        glm::vec3 currentPos = transform->GetPosition();

        bool needNewDestination = !ctx.State.HasCustomTarget;

        if (ctx.State.HasCustomTarget) {
            float distToTarget = glm::distance(currentPos, ctx.State.CustomTarget);
            constexpr float arrivalThreshold = 0.5f;
            if (distToTarget <= arrivalThreshold) {
                ctx.State.HasCustomTarget = false;
                navAgent->HasDestination = false;
                return BTStatus::Success;
            }

            if (navAgent->CurrentSpeed < 0.05f) {
                ctx.State.Timer += ctx.DeltaTime;
                if (ctx.State.Timer > 2.0f) {
                    needNewDestination = true;
                    ctx.State.Timer = 0.0f;
                }
            } else {
                ctx.State.Timer = 0.0f;
            }
        }

        if (needNewDestination) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dist(-radius, radius);

            glm::vec3 targetPos;
            bool foundValidPath = false;

            for (int attempt = 0; attempt < 5; ++attempt) {
                targetPos = currentPos + glm::vec3(dist(gen), 0, dist(gen));

                if (ctx.Navmesh) {
                    if (!ctx.Navmesh->IsWalkable(targetPos)) {
                        targetPos = ctx.Navmesh->FindNearestPoint(targetPos);
                    }
                    auto path = ctx.Navmesh->FindPath(currentPos, targetPos);
                    if (!path.empty()) {
                        foundValidPath = true;
                        break;
                    }
                } else {
                    foundValidPath = true;
                    break;
                }
            }

            if (!foundValidPath) {
                return BTStatus::Success;
            }

            ctx.State.CustomTarget = targetPos;
            ctx.State.HasCustomTarget = true;

            navAgent->Destination = targetPos;
            navAgent->HasDestination = true;
            navAgent->DestinationChanged = true;
        }

        return BTStatus::Running;
    }
};

using NavmeshBTNodeTypes = fsm::TypeList<
    IsMovingNode,
    MoveToTargetNode,
    RandomWanderNode,
    IdleNode
>;
