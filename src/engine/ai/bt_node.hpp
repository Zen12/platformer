#pragma once
#include "bt_status.hpp"
#include "bt_def.hpp"
#include <entt/entt.hpp>
#include <string_view>

class BehaviorTreeComponent;
class GridNavmesh;

struct BTContext {
    BehaviorTreeComponent& BT;
    entt::registry& Registry;
    entt::entity Entity;
    float DeltaTime;
    BTNodeState& State;
    const BTNodeDef& Def;
    GridNavmesh* Navmesh = nullptr;
};

struct IBTNode {
    virtual BTStatus Execute(BTContext& ctx) = 0;
    virtual ~IBTNode() = default;
};
