#pragma once
#include "bt_node.hpp"
#include "bt_component.hpp"
#include "plugins/core/src/navmesh_agent/navmesh_agent_component.hpp"
#include "type_list.hpp"

struct AttackNode final : IBTNode {
    BTStatus Execute(BTContext& ctx) override {
        auto* navAgent = ctx.Registry.try_get<NavmeshAgentComponent>(ctx.Entity);
        if (navAgent) {
            navAgent->HasDestination = false;
            navAgent->DestinationChanged = true;
        }

        ctx.BT.IsAttacking = true;

        ctx.State.Timer += ctx.DeltaTime;
        if (ctx.State.Timer >= ctx.Def.Param1) {
            ctx.State.Timer = 0.0f;
            ctx.BT.IsAttacking = false;
            return BTStatus::Success;
        }

        return BTStatus::Running;
    }
};

using CombatBTNodeTypes = fsm::TypeList<AttackNode>;
