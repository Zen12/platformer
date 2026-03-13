#pragma once
#include "bt_node.hpp"
#include "bt_component.hpp"
#include "type_list.hpp"

struct WaitNode final : IBTNode {
    BTStatus Execute(BTContext& ctx) override {
        ctx.State.Float("timer") += ctx.DeltaTime;
        if (ctx.State.GetFloat("timer") >= ctx.Def.GetFloat("duration")) {
            ctx.State.Float("timer") = 0.0f;
            return BTStatus::Success;
        }
        return BTStatus::Running;
    }
};

struct HasTargetNode final : IBTNode {
    BTStatus Execute(BTContext& ctx) override {
        return ctx.BT.HasTarget ? BTStatus::Success : BTStatus::Failure;
    }
};

struct ClearTargetNode final : IBTNode {
    BTStatus Execute(BTContext& ctx) override {
        ctx.BT.HasTarget = false;
        ctx.BT.TargetEntity = entt::null;
        return BTStatus::Success;
    }
};

using CoreBTNodeTypes = fsm::TypeList<
    WaitNode,
    HasTargetNode,
    ClearTargetNode
>;
