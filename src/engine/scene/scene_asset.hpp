#pragma once
#include <string>
#include <vector>
#include "../entity/entity_serialization.hpp"

struct SceneAsset
{
    std::string Type;
    std::vector<EntitySerialization> Entities;
};
