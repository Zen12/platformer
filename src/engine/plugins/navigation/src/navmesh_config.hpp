#pragma once
#include <string>
#include <vector>
#include <optional>
#include <glm/glm.hpp>
#include "guid.hpp"

struct NavmeshConfig {
    int Width = 50;
    int Height = 50;
    float CellSize = 1.0f;
    glm::vec3 Origin = glm::vec3(-25.0f, 0.0f, -25.0f);
    std::vector<std::vector<int>> WalkabilityGrid;
    int MaxAgents = 2000;
    float ElevationHeight = 1.0f;
    std::optional<Guid> GroundMaterialGuid;
    std::optional<Guid> WallMaterialGuid;
};
