#pragma once
#include <array>
#include <string>
#include <vector>

class ConnectionSerialization final {
public:
    std::array<std::string, 2> Nodes{};
    std::vector<std::string> ConditionGuids{};
    int ConditionType{0};
};
