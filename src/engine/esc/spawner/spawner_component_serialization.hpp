#pragma once
#include "../../entity/component_serialization.hpp"
#include "../../system/guid.hpp"
#include <vector>
#include <glm/glm.hpp>

struct SpawnerComponentSerialization final : public ComponentSerialization {
    Guid PrefabGuid;
    int SpawnCount = 1;
    std::vector<glm::vec3> SpawnPositions;
    bool SpawnOnNavmesh = true;
    bool SpawnOnAllCells = false;
    float ElevationHeight = 1.0f;
    float YOffset = 0.0f;

    ~SpawnerComponentSerialization() override = default;
};
