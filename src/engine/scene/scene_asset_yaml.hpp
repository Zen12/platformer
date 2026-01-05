#pragma once
#include <yaml-cpp/yaml.h>
#include "scene_asset.hpp"
#include "../entity/entity_serialization_yaml.hpp"

namespace YAML {
    template <>
    struct convert<NavmeshConfig>
    {
        static bool decode(const Node &node, NavmeshConfig &rhs)
        {
            if (node["width"]) rhs.Width = node["width"].as<int>();
            if (node["height"]) rhs.Height = node["height"].as<int>();
            if (node["cell_size"]) rhs.CellSize = node["cell_size"].as<float>();
            if (node["origin"]) {
                auto origin = node["origin"].as<std::vector<float>>();
                if (origin.size() == 3) {
                    rhs.Origin = glm::vec3(origin[0], origin[1], origin[2]);
                }
            }
            if (node["walkability_grid"]) {
                rhs.WalkabilityGrid = node["walkability_grid"].as<std::vector<std::vector<int>>>();
            }
            if (node["max_agents"]) {
                rhs.MaxAgents = node["max_agents"].as<int>();
            }
            return true;
        }
    };

    template <>
    struct convert<SkyboxConfig>
    {
        static bool decode(const Node &node, SkyboxConfig &rhs)
        {
            if (node["material"]) {
                rhs.MaterialGuid = node["material"].as<std::string>();
            }
            return true;
        }
    };

    template <>
    struct convert<SceneAsset>
    {
        static bool decode(const Node &node, SceneAsset &rhs)
        {
            rhs.Type = node["type"].as<std::string>();
            rhs.Entities = node["entities"].as<std::vector<EntitySerialization>>();
            if (node["navmesh"]) {
                rhs.Navmesh = node["navmesh"].as<NavmeshConfig>();
            }
            if (node["skybox"]) {
                rhs.Skybox = node["skybox"].as<SkyboxConfig>();
            }
            return true;
        }
    };
}
