#pragma once
#include <vector>
#include <string>
#include "action/action_serialization_data.hpp"

class StateNodeSerialization final {
public:
    std::string Guid;
    std::vector<ActionSerializationData> ActionData{};
};
