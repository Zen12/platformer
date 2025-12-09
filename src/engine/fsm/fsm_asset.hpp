#pragma once
#include "condition/condition_serialization.hpp"
#include "connection/connection_serialization.hpp"
#include "node/node_serialization.hpp"

struct FsmAsset final {
    std::string Name{};
    std::string StartNode{};
    std::vector<std::unique_ptr<StateNodeSerialization>> StateNodeSerialization{};
    std::vector<ConnectionSerialization> ConnectionSerialization{};
    std::vector<std::unique_ptr<ConditionSerialization>> ConditionSerialization{};
};