#pragma once
#include <string>
#include <vector>
#include <optional>
#include <glm/glm.hpp>
#include "../entity/entity_serialization.hpp"

struct NavmeshConfig {
    int Width = 50;
    int Height = 50;
    float CellSize = 1.0f;
    glm::vec3 Origin = glm::vec3(-25.0f, 0.0f, -25.0f);
    std::vector<std::vector<int>> WalkabilityGrid; // Optional: 2D grid of 0 (unwalkable) or 1 (walkable)
};

struct SceneAsset
{
    std::string Type;
    std::vector<EntitySerialization> Entities;
    std::optional<NavmeshConfig> Navmesh;
};
