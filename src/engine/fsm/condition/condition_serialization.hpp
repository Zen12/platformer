#pragma once
#include <string>


class ConditionSerialization {

public:
    virtual ~ConditionSerialization() = default;

    std::string Type;
    std::string Guid;
};
