#pragma once
#include "bt_def.hpp"
#include "bt_component.hpp"
#include "../esc/navmesh_agent/navmesh_agent_component.hpp"
#include "../esc/transform/transform_component.hpp"
#include "../esc/tag/tag_component.hpp"
#include <glm/glm.hpp>
#include <random>
#include <iostream>
#include <limits>
#include <entt/entt.hpp>

// Enable/disable behavior tree debug logging
#define BT_DEBUG 0

#if BT_DEBUG
#define BT_LOG(msg) std::cout << "[BT] " << msg << std::endl
#define BT_LOG_NODE(nodeType, status) std::cout << "[BT] Node: " << NodeTypeToString(nodeType) << " -> " << StatusToString(status) << std::endl
#else
#define BT_LOG(msg)
#define BT_LOG_NODE(nodeType, status)
#endif

inline const char* NodeTypeToString(BTNodeType type) {
    switch (type) {
        case BTNodeType::Sequence: return "Sequence";
        case BTNodeType::Selector: return "Selector";
        case BTNodeType::Parallel: return "Parallel";
        case BTNodeType::Inverter: return "Inverter";
        case BTNodeType::Repeater: return "Repeater";
        case BTNodeType::UntilFail: return "UntilFail";
        case BTNodeType::CheckDistance: return "CheckDistance";
        case BTNodeType::HasTarget: return "HasTarget";
        case BTNodeType::CheckHealth: return "CheckHealth";
        case BTNodeType::IsMoving: return "IsMoving";
        case BTNodeType::MoveTo: return "MoveTo";
        case BTNodeType::MoveToTarget: return "MoveToTarget";
        case BTNodeType::Attack: return "Attack";
        case BTNodeType::Idle: return "Idle";
        case BTNodeType::Wait: return "Wait";
        case BTNodeType::RandomWander: return "RandomWander";
        case BTNodeType::SetTarget: return "SetTarget";
        case BTNodeType::FindTargetByTag: return "FindTargetByTag";
        case BTNodeType::HasTargetInRange: return "HasTargetInRange";
        case BTNodeType::ClearTarget: return "ClearTarget";
        default: return "Unknown";
    }
}

inline const char* StatusToString(BTStatus status) {
    switch (status) {
        case BTStatus::Success: return "SUCCESS";
        case BTStatus::Failure: return "FAILURE";
        case BTStatus::Running: return "RUNNING";
        default: return "UNKNOWN";
    }
}

class BTExecutor {
public:
    static BTStatus Execute(
        BehaviorTreeComponent& bt,
        NavmeshAgentComponent* navAgent,
        TransformComponentV2* transform,
        float deltaTime,
        entt::registry& registry,
        entt::entity selfEntity
    ) {
        if (!bt.TreeDef || bt.TreeDef->Nodes.empty()) {
            BT_LOG("No tree definition");
            return BTStatus::Failure;
        }

        BT_LOG("--- Execute tree: " << bt.TreeDef->Name << " ---");

        // Process the execution stack
        while (!bt.State.IsEmpty()) {
            auto& current = bt.State.Current();
            const auto& node = bt.TreeDef->GetNode(current.NodeIndex);

            BT_LOG("Processing node[" << current.NodeIndex << "]: " << NodeTypeToString(node.Type));

            BTStatus result = ExecuteNode(bt, node, current, navAgent, transform, deltaTime, registry, selfEntity);

            BT_LOG_NODE(node.Type, result);

            if (result == BTStatus::Running) {
                return BTStatus::Running;
            }

            // Node completed, handle parent
            bt.State.Pop();
            if (!bt.State.IsEmpty()) {
                auto& parent = bt.State.Current();
                const auto& parentNode = bt.TreeDef->GetNode(parent.NodeIndex);

                if (!HandleChildResult(bt, parentNode, parent, result)) {
                    continue;  // Parent needs to continue processing
                }
            }
        }

        // Tree completed, reset for next tick
        BT_LOG("Tree completed, resetting");
        bt.State.Reset();
        return BTStatus::Success;
    }

private:
    static BTStatus ExecuteNode(
        BehaviorTreeComponent& bt,
        const BTNodeDef& node,
        BTNodeState& state,
        NavmeshAgentComponent* navAgent,
        TransformComponentV2* transform,
        float deltaTime,
        entt::registry& registry,
        entt::entity selfEntity
    ) {
        switch (node.Type) {
            // Composite nodes
            case BTNodeType::Sequence:
            case BTNodeType::Selector:
                return ExecuteComposite(bt, node, state);

            // Condition nodes
            case BTNodeType::CheckDistance:
                return CheckDistance(bt, transform, node.Param1);
            case BTNodeType::HasTarget:
                return bt.HasTarget ? BTStatus::Success : BTStatus::Failure;
            case BTNodeType::IsMoving:
                return (navAgent && navAgent->CurrentSpeed > 0.1f)
                    ? BTStatus::Success : BTStatus::Failure;
            case BTNodeType::HasTargetInRange:
                return HasTargetInRange(bt, transform, registry, node.Param1);

            // Target finding nodes
            case BTNodeType::FindTargetByTag:
                return FindTargetByTag(bt, transform, registry, selfEntity, node.StringParam, node.Param1);
            case BTNodeType::ClearTarget:
                bt.HasTarget = false;
                bt.TargetEntity = entt::null;
                return BTStatus::Success;

            // Action nodes
            case BTNodeType::MoveToTarget:
                return MoveToTarget(bt, navAgent, registry);
            case BTNodeType::RandomWander:
                return RandomWander(bt, navAgent, transform, node.Param1);
            case BTNodeType::Wait:
                return Wait(bt, state, deltaTime, node.Param1);
            case BTNodeType::Idle:
                return BTStatus::Success;
            case BTNodeType::Attack:
                return BTStatus::Success;  // TODO: Implement attack

            default:
                return BTStatus::Failure;
        }
    }

    static BTStatus ExecuteComposite(
        BehaviorTreeComponent& bt,
        const BTNodeDef& node,
        BTNodeState& state
    ) {
        if (state.ChildProgress < node.ChildCount) {
            uint16_t childIndex = node.FirstChildIndex + state.ChildProgress;
            bt.State.Push(childIndex);
            return BTStatus::Running;
        }

        // All children processed
        return (node.Type == BTNodeType::Sequence)
            ? BTStatus::Success
            : BTStatus::Failure;
    }

    static bool HandleChildResult(
        [[maybe_unused]] BehaviorTreeComponent& bt,
        const BTNodeDef& parentNode,
        BTNodeState& parentState,
        BTStatus childResult
    ) {
        if (parentNode.Type == BTNodeType::Sequence) {
            if (childResult == BTStatus::Failure) {
                return true;  // Sequence fails on first failure
            }
            parentState.ChildProgress++;
            return false;  // Continue to next child
        }
        else if (parentNode.Type == BTNodeType::Selector) {
            if (childResult == BTStatus::Success) {
                return true;  // Selector succeeds on first success
            }
            parentState.ChildProgress++;
            return false;  // Try next child
        }
        return true;
    }

    // Condition implementations
    static BTStatus CheckDistance(
        const BehaviorTreeComponent& bt,
        const TransformComponentV2* transform,
        float threshold
    ) {
        if (!bt.HasTarget || !transform) return BTStatus::Failure;
        float dist = glm::distance(transform->GetPosition(), bt.TargetPosition);
        return (dist <= threshold) ? BTStatus::Success : BTStatus::Failure;
    }

    // Target finding implementations
    static BTStatus FindTargetByTag(
        BehaviorTreeComponent& bt,
        const TransformComponentV2* transform,
        entt::registry& registry,
        entt::entity selfEntity,
        const std::string& tag,
        float searchRadius
    ) {
        if (!transform) return BTStatus::Failure;

        const auto myPos = transform->GetPosition();
        float bestDist = searchRadius > 0 ? searchRadius : std::numeric_limits<float>::max();
        entt::entity bestTarget = entt::null;
        glm::vec3 bestPos{0};

        // Search for entities with matching tag
        auto view = registry.view<TagComponent, TransformComponentV2>();
        for (auto entity : view) {
            if (entity == selfEntity) continue;

            const auto& tagComp = view.get<TagComponent>(entity);
            if (tagComp.GetTag() != tag) continue;

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
            bt.TargetEntity = bestTarget;
            bt.TargetPosition = bestPos;
            bt.HasTarget = true;
            bt.TargetTag = tag;
            BT_LOG("FindTargetByTag: found '" << tag << "' at distance " << bestDist);
            return BTStatus::Success;
        }

        BT_LOG("FindTargetByTag: no '" << tag << "' found within radius " << searchRadius);
        return BTStatus::Failure;
    }

    static BTStatus HasTargetInRange(
        const BehaviorTreeComponent& bt,
        const TransformComponentV2* transform,
        entt::registry& registry,
        float range
    ) {
        if (!bt.HasTarget || !transform) return BTStatus::Failure;
        if (bt.TargetEntity == entt::null) return BTStatus::Failure;

        // Check if target entity still exists and get its current position
        if (!registry.valid(bt.TargetEntity)) return BTStatus::Failure;

        auto* targetTransform = registry.try_get<TransformComponentV2>(bt.TargetEntity);
        if (!targetTransform) return BTStatus::Failure;

        float dist = glm::distance(transform->GetPosition(), targetTransform->GetPosition());
        BT_LOG("HasTargetInRange: dist=" << dist << " range=" << range);
        return (dist <= range) ? BTStatus::Success : BTStatus::Failure;
    }

    // Action implementations
    static BTStatus MoveToTarget(
        BehaviorTreeComponent& bt,
        NavmeshAgentComponent* navAgent,
        entt::registry& registry
    ) {
        if (!bt.HasTarget || !navAgent) return BTStatus::Failure;

        // Update target position from entity if we have one
        if (bt.TargetEntity != entt::null && registry.valid(bt.TargetEntity)) {
            auto* targetTransform = registry.try_get<TransformComponentV2>(bt.TargetEntity);
            if (targetTransform) {
                bt.TargetPosition = targetTransform->GetPosition();
            }
        }

        navAgent->Destination = bt.TargetPosition;
        navAgent->HasDestination = true;
        navAgent->DestinationChanged = true;

        // Check if arrived (close enough to target)
        if (navAgent->CurrentSpeed < 0.1f) {
            BT_LOG("MoveToTarget: arrived or stuck");
            return BTStatus::Success;
        }
        return BTStatus::Running;
    }

    static BTStatus RandomWander(
        BehaviorTreeComponent& bt,
        NavmeshAgentComponent* navAgent,
        const TransformComponentV2* transform,
        float radius
    ) {
        if (!navAgent || !transform) {
            BT_LOG("RandomWander: missing navAgent or transform");
            return BTStatus::Failure;
        }

        // Generate random point if not currently moving
        if (!navAgent->HasDestination || navAgent->CurrentSpeed < 0.1f) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dist(-radius, radius);

            bt.TargetPosition = transform->GetPosition() + glm::vec3(dist(gen), 0, dist(gen));
            bt.HasTarget = true;

            navAgent->Destination = bt.TargetPosition;
            navAgent->HasDestination = true;
            navAgent->DestinationChanged = true;

            BT_LOG("RandomWander: new target (" << bt.TargetPosition.x << ", " << bt.TargetPosition.z << ")");
        } else {
            BT_LOG("RandomWander: moving, speed=" << navAgent->CurrentSpeed);
        }

        return BTStatus::Running;
    }

    static BTStatus Wait(
        [[maybe_unused]] BehaviorTreeComponent& bt,
        BTNodeState& state,
        float deltaTime,
        float duration
    ) {
        state.Timer += deltaTime;
        BT_LOG("Wait: " << state.Timer << "/" << duration);
        if (state.Timer >= duration) {
            state.Timer = 0.0f;
            BT_LOG("Wait: completed");
            return BTStatus::Success;
        }
        return BTStatus::Running;
    }
};
