#pragma once

#include <memory>
#include <string>

#include "fsm_data.hpp"
#include "fsm_asset.hpp"
#include "../register/fsm_action_registry.hpp"
#include "../register/fsm_condition_registry.hpp"
#include "engine_context.hpp"

class FsmBuilder final {
public:
    static FsmData Build(
        const FsmAsset& fsmAsset,
        const FsmActionRegistry& actionRegistry,
        const FsmConditionRegistry& conditionRegistry,
        const EngineContext& context
    ) {
        std::unordered_map<std::string, StateNode> stateNodes;
        std::vector<Connection> connections;
        std::unordered_map<std::string, std::unique_ptr<ConditionBase>> conditions;

        for (const auto& nodeSer : fsmAsset.StateNodeSerialization) {
            std::vector<std::unique_ptr<Action>> actions;

            for (const auto& actionSer : nodeSer->ActionData) {
                if (auto action = actionRegistry.Build(actionSer->Type, actionSer.get(), context)) {
                    actions.push_back(std::move(action));
                }
            }

            StateNode node(nodeSer->Guid, std::move(actions));
            stateNodes.emplace(nodeSer->Guid, std::move(node));
        }

        for (const auto& connSer : fsmAsset.ConnectionSerialization) {
            Connection conn;
            conn.Nodes = connSer.Nodes;
            conn.ConditionGuids = connSer.ConditionGuids;
            conn.ConditionType = static_cast<::ConditionType>(connSer.ConditionType);
            connections.push_back(conn);
        }

        for (const auto& condSer : fsmAsset.ConditionSerialization) {
            if (!condSer) continue;
            if (auto cond = conditionRegistry.Build(condSer->Type, condSer.get(), context)) {
                conditions[condSer->Guid] = std::move(cond);
            }
        }

        return FsmData{
            std::move(stateNodes),
            std::move(connections),
            std::move(conditions),
            fsmAsset.StartNode
        };
    }
};
