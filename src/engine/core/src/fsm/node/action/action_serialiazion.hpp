#pragma once
#include <string>


class ActionSerialization {
public:
    std::string Type{};
    virtual ~ActionSerialization() = default;
};
