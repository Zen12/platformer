#pragma once

#include "../../asset/serialization/serialization_component.hpp"

struct GridPrefabSpawnerSerialization final : public ComponentSerialization
{
    std::string prefabId;
    std::string gridTag;
};