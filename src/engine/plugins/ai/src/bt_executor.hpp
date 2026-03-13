#pragma once
#include "bt_def.hpp"
#include "bt_blackboard.hpp"
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
    BTBlackboard _blackboard;

public:
    template<typename T>
    void Set(T value) { _blackboard.Set(std::move(value)); }

    BTStatus Execute(
        BehaviorTreeComponent& bt,
        const BTNodeRegistry& nodeRegistry,
        entt::entity entity
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

            BTStatus result;
            if (node.Composite != BTCompositeType::NONE) {
                result = ExecuteComposite(bt, node, current);
            } else {
                BTContext ctx(bt, entity, current, node, _blackboard);
                result = ExecuteNode(ctx, nodeRegistry);
            }

            BT_LOG_NODE(node.Type, result);

            if (result == BTStatus::Running) {
                return BTStatus::Running;
            }

            bt.State.Pop();

            while (!bt.State.IsEmpty()) {
                auto& parent = bt.State.Current();
                const auto& parentNode = bt.TreeDef->GetNode(parent.NodeIndex);

                if (!HandleChildResult(parentNode, parent, result)) {
                    break;
                }

                if (parentNode.Composite == BTCompositeType::SEQUENCE) {
                    result = BTStatus::Failure;
                } else if (parentNode.Composite == BTCompositeType::SELECTOR) {
                    result = (result == BTStatus::Success) ? BTStatus::Success : BTStatus::Failure;
                } else if (parentNode.Composite == BTCompositeType::REPEATER) {
                    result = BTStatus::Failure;
                }

                BT_LOG("Propagating " << StatusToString(result) << " from composite " << static_cast<int>(parentNode.Composite));
                bt.State.Pop();
            }
        }

        BT_LOG("Tree completed, resetting");
        bt.State.Reset();
        return BTStatus::Success;
    }

private:
    static BTStatus ExecuteComposite(
        BehaviorTreeComponent& bt,
        const BTNodeDef& node,
        BTNodeState& state
    ) {
        if (state.ChildProgress < node.ChildIndices.size()) {
            uint16_t childIndex = node.ChildIndices[state.ChildProgress];
            bt.State.Push(childIndex);
            return BTStatus::Running;
        }
        switch (node.Composite) {
            case BTCompositeType::SEQUENCE: return BTStatus::Success;
            case BTCompositeType::SELECTOR: return BTStatus::Failure;
            case BTCompositeType::REPEATER: return BTStatus::Success;
            default: return BTStatus::Failure;
        }
    }

    static BTStatus ExecuteNode(BTContext& ctx, const BTNodeRegistry& nodeRegistry) {
        auto* node = nodeRegistry.GetNode(ctx.Def.Type);
        if (node) {
            return node->Execute(ctx);
        }
        return BTStatus::Failure;
    }

    static bool HandleChildResult(
        const BTNodeDef& parentNode,
        BTNodeState& parentState,
        BTStatus childResult
    ) {
        switch (parentNode.Composite) {
            case BTCompositeType::SEQUENCE:
                if (childResult == BTStatus::Failure) return true;
                parentState.ChildProgress++;
                return false;
            case BTCompositeType::SELECTOR:
                if (childResult == BTStatus::Success) return true;
                parentState.ChildProgress++;
                return false;
            case BTCompositeType::REPEATER:
                if (childResult == BTStatus::Failure) return true;
                parentState.ChildProgress = 0;
                return false;
            default:
                return true;
        }
    }
};
