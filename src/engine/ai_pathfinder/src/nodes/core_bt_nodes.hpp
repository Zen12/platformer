#pragma once
#include "bt_node.hpp"
#include "bt_component.hpp"
#include "../../../fsm/type_list.hpp"

struct SequenceNode final : IBTNode {
    BTStatus Execute(BTContext& ctx) override {
        if (ctx.State.ChildProgress < ctx.Def.ChildIndices.size()) {
            uint16_t childIndex = ctx.Def.ChildIndices[ctx.State.ChildProgress];
            ctx.BT.State.Push(childIndex);
            return BTStatus::Running;
        }
        return BTStatus::Success;
    }
};

struct SelectorNode final : IBTNode {
    BTStatus Execute(BTContext& ctx) override {
        if (ctx.State.ChildProgress < ctx.Def.ChildIndices.size()) {
            uint16_t childIndex = ctx.Def.ChildIndices[ctx.State.ChildProgress];
            ctx.BT.State.Push(childIndex);
            return BTStatus::Running;
        }
        return BTStatus::Failure;
    }
};

struct WaitNode final : IBTNode {
    BTStatus Execute(BTContext& ctx) override {
        ctx.State.Timer += ctx.DeltaTime;
        if (ctx.State.Timer >= ctx.Def.Param1) {
            ctx.State.Timer = 0.0f;
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
    SequenceNode,
    SelectorNode,
    WaitNode,
    HasTargetNode,
    ClearTargetNode
>;
