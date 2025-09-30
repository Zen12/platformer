#pragma once

#include "../../entity/component_serialization.hpp"

struct GridPrefabSpawnerSerialization final : public ComponentSerialization
{
    std::string prefabId;
    std::string gridTag;
};