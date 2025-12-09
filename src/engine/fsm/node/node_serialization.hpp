#pragma once
#include <vector>

#include "action/action_serialiazion.hpp"


class StateNodeSerialization final {
public:
    std::string Guid;
    std::vector<StateNode::AllActionVariants> Actions{};
};
