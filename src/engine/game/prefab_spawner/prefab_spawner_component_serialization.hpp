#pragma once

#include "../../entity/component_serialization.hpp"

struct PrefabSpawnerSerialization final : public ComponentSerialization
{
    std::string prefabId;
    float spawnTime = 0;
    int maxSpawn;
    std::vector<glm::vec3> positions;
};