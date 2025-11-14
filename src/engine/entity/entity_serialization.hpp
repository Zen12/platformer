#pragma once

#include "component_serialization.hpp"

struct EntitySerialization
{
    std::string Creator{}; // remove
    std::string Tag{};
    std::string Guid{}; // remove
    int Layer{};
    std::vector<std::shared_ptr<ComponentSerialization>> Components;
};
