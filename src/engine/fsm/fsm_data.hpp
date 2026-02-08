#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "node/node.hpp"
#include "connection/connection.hpp"
#include "condition/condition.hpp"

struct FsmData final {
    const std::unordered_map<std::string, StateNode> StateNodes;
    const std::vector<Connection> Connections;
    const std::unordered_map<std::string, AllConditionVariants> Conditions;
    const std::string StartNode;

    FsmData(const FsmData&) = delete;
    FsmData& operator=(const FsmData&) = delete;
    FsmData(FsmData&&) = default;
    FsmData& operator=(FsmData&&) = delete;

    FsmData(
        std::unordered_map<std::string, StateNode> stateNodes,
        std::vector<Connection> connections,
        std::unordered_map<std::string, AllConditionVariants> conditions,
        std::string startNode
    ) : StateNodes(std::move(stateNodes)),
        Connections(std::move(connections)),
        Conditions(std::move(conditions)),
        StartNode(std::move(startNode)) {}
};
