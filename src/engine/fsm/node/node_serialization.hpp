#pragma once
#include <vector>
#include <string>
#include <memory>
#include "action/action_serialiazion.hpp"

class StateNodeSerialization final {
public:
    std::string Guid;
    std::vector<std::unique_ptr<ActionSerialization>> ActionData{};
};
