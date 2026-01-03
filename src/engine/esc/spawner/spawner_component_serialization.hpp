#pragma once
#include "../../entity/component_serialization.hpp"
#include <string>
#include <vector>
#include <glm/glm.hpp>

struct SpawnerComponentSerialization final : public ComponentSerialization {
    std::string PrefabGuid;
    int SpawnCount = 1;
    std::vector<glm::vec3> SpawnPositions;
    bool SpawnOnNavmesh = true;
    bool SpawnOnAllCells = false;

    ~SpawnerComponentSerialization() override = default;
};
