#pragma once
#include "bt_def.hpp"
#include "bt_component.hpp"
#include "bt_node.hpp"
#include "bt_node_registry.hpp"
#include <iostream>

#define BT_DEBUG 0

#if BT_DEBUG
#define BT_LOG(msg) std::cout << "[BT] " << msg << std::endl
#define BT_LOG_NODE(nodeType, status) std::cout << "[BT] Node: " << nodeType << " -> " << StatusToString(status) << std::endl
#else
#define BT_LOG(msg)
#define BT_LOG_NODE(nodeType, status)
#endif

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
        float deltaTime,
        entt::registry& registry,
        entt::entity selfEntity,
        GridNavmesh* navmesh = nullptr
    ) {
        if (!bt.TreeDef || bt.TreeDef->Nodes.empty()) {
            BT_LOG("No tree definition");
            return BTStatus::Failure;
        }

        BT_LOG("--- Execute tree: " << bt.TreeDef->Name << " ---");

        while (!bt.State.IsEmpty()) {
            auto& current = bt.State.Current();
            const auto& node = bt.TreeDef->GetNode(current.NodeIndex);

            BT_LOG("Processing node[" << current.NodeIndex << "]: " << node.Type);

            BTContext ctx{bt, registry, selfEntity, deltaTime, current, node, navmesh};
            BTStatus result = ExecuteNode(ctx);

            BT_LOG_NODE(node.Type, result);

            if (result == BTStatus::Running) {
                return BTStatus::Running;
            }

            bt.State.Pop();

            while (!bt.State.IsEmpty()) {
                auto& parent = bt.State.Current();
                const auto& parentNode = bt.TreeDef->GetNode(parent.NodeIndex);

                if (!HandleChildResult(bt, parentNode, parent, result)) {
                    break;
                }

                if (parentNode.Type == "sequence") {
                    result = BTStatus::Failure;
                } else if (parentNode.Type == "selector") {
                    result = (result == BTStatus::Success) ? BTStatus::Success : BTStatus::Failure;
                }

                BT_LOG("Propagating " << StatusToString(result) << " from " << parentNode.Type);
                bt.State.Pop();
            }
        }

        BT_LOG("Tree completed, resetting");
        bt.State.Reset();
        return BTStatus::Success;
    }

private:
    static BTStatus ExecuteNode(BTContext& ctx) {
        auto* node = BTNodeRegistry::Instance().GetNode(ctx.Def.Type);
        if (node) {
            return node->Execute(ctx);
        }
        return BTStatus::Failure;
    }

    static bool HandleChildResult(
        [[maybe_unused]] BehaviorTreeComponent& bt,
        const BTNodeDef& parentNode,
        BTNodeState& parentState,
        BTStatus childResult
    ) {
        if (parentNode.Type == "sequence") {
            if (childResult == BTStatus::Failure) {
                return true;
            }
            parentState.ChildProgress++;
            return false;
        }
        else if (parentNode.Type == "selector") {
            if (childResult == BTStatus::Success) {
                return true;
            }
            parentState.ChildProgress++;
            return false;
        }
        return true;
    }
};
