#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "node/node.hpp"
#include "connection/connection.hpp"
#include "condition/condition_base.hpp"

struct FsmData final {
    std::unordered_map<std::string, StateNode> StateNodes;
    std::vector<Connection> Connections;
    std::unordered_map<std::string, std::unique_ptr<ConditionBase>> Conditions;
    std::string StartNode;

    FsmData(const FsmData&) = delete;
    FsmData& operator=(const FsmData&) = delete;
    FsmData(FsmData&&) = default;
    FsmData& operator=(FsmData&&) = delete;

    FsmData(
        std::unordered_map<std::string, StateNode> stateNodes,
        std::vector<Connection> connections,
        std::unordered_map<std::string, std::unique_ptr<ConditionBase>> conditions,
        std::string startNode
    ) : StateNodes(std::move(stateNodes)),
        Connections(std::move(connections)),
        Conditions(std::move(conditions)),
        StartNode(std::move(startNode)) {}
};
