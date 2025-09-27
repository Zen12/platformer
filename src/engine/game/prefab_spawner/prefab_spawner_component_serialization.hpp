#pragma once

#include "../../asset/serialization/serialization_component.hpp"

struct PrefabSpawnerSerialization final : public ComponentSerialization
{
    std::string prefabId;
    float spawnTime = 0;
    int maxSpawn;
    std::vector<glm::vec3> positions;
};