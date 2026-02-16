#pragma once
#include "bt_def.hpp"
#include "bt_component_serialization.hpp"
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include "guid.hpp"

class BehaviorTreeComponent final {
public:
    Guid TreeGuid;
    std::shared_ptr<BehaviorTreeDef> TreeDef;
    BTExecutionState State;

    uint8_t TickPriority = 0;
    uint8_t FrameOffset = 0;

    glm::vec3 TargetPosition = glm::vec3(0);
    entt::entity TargetEntity = entt::null;
    bool HasTarget = false;
    float WaitTimer = 0.0f;
    std::string TargetTag;

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
