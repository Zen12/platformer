#pragma once

#include "component_serialization.hpp"

struct EntitySerialization
{
    std::string Creator{};
    std::string Tag;
    std::string Guid;
    int Layer;
    std::vector<std::shared_ptr<ComponentSerialization>> Components;
};
