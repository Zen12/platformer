#pragma once
#include "bt_status.hpp"
#include "bt_def.hpp"
#include "bt_blackboard.hpp"
#include <entt/entt.hpp>

class BehaviorTreeComponent;
class GridNavmesh;

struct BTContext {
    BehaviorTreeComponent& BT;
    entt::registry& Registry;
    entt::entity Entity;
    float DeltaTime;
    BTNodeState& State;
    const BTNodeDef& Def;
    GridNavmesh* Navmesh;
    const BTBlackboard& Blackboard;

    BTContext(BehaviorTreeComponent& bt, entt::entity entity,
             BTNodeState& state, const BTNodeDef& def, const BTBlackboard& blackboard)
        : BT(bt),
          Registry(*blackboard.Get<entt::registry*>()),
          Entity(entity),
          DeltaTime(blackboard.Get<float>()),
          State(state),
          Def(def),
          Navmesh(blackboard.Get<GridNavmesh*>()),
          Blackboard(blackboard) {}
};

struct IBTNode {
    virtual BTStatus Execute(BTContext& ctx) = 0;
    virtual ~IBTNode() = default;
};
