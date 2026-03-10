#pragma once
#include <string>

struct ComponentSerialization
{
    std::string ComponentType{};
    virtual ~ComponentSerialization() = default;
};
