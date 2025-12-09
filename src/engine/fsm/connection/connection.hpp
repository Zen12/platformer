#pragma once
#include <array>
#include <string>
#include <vector>


enum class ConditionType {
    All = 0,
    AtLeastOne = 1
};

class Connection final {
public:
    std::array<std::string, 2> Nodes;
    std::vector<std::string> ConditionGuids;
    ConditionType ConditionType;
};

