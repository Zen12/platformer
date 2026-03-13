#pragma once
#include <string>
#include <vector>
#include <optional>
#include <glm/glm.hpp>
#include "entity/entity_serialization.hpp"
#include "guid.hpp"
#include "navmesh_config.hpp"

struct SkyboxConfig {
    Guid MaterialGuid;
};

struct SceneAsset
{
    std::string Type;
    std::vector<EntitySerialization> Entities;
    std::optional<NavmeshConfig> Navmesh;
    std::optional<SkyboxConfig> Skybox;
};
