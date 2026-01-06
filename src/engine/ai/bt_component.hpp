#pragma once
#include "bt_def.hpp"
#include "bt_component_serialization.hpp"
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

class BehaviorTreeComponent final {
public:
    // Tree GUID for lazy loading
    std::string TreeGuid;

    // Shared tree definition (many entities can share same tree type)
    std::shared_ptr<BehaviorTreeDef> TreeDef;

    // Per-entity execution state
    BTExecutionState State;

    // LOD settings
    uint8_t TickPriority = 0;   // 0=every frame, 1=every 2nd, 2=every 4th...
    uint8_t FrameOffset = 0;    // Stagger ticks across frames

    // Blackboard data (entity-specific)
    glm::vec3 TargetPosition = glm::vec3(0);
    entt::entity TargetEntity = entt::null;
    bool HasTarget = false;
    float WaitTimer = 0.0f;
    std::string TargetTag;  // Tag to search for

    // Attack state
    bool IsAttacking = false;
    float AttackTimer = 0.0f;

    explicit BehaviorTreeComponent(const BehaviorTreeComponentSerialization& serialization)
        : TreeGuid(serialization.TreeGuid), TickPriority(serialization.TickPriority) {
        State.Reset();
    }

    void Reset() {
        State.Reset();
        HasTarget = false;
        WaitTimer = 0.0f;
        IsAttacking = false;
        AttackTimer = 0.0f;
    }
};
