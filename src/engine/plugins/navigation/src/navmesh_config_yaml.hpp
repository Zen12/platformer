#pragma once
#include <yaml-cpp/yaml.h>
#include "navmesh_config.hpp"
#include "guid_yaml.hpp"

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
            if (node["elevation_height"]) {
                rhs.ElevationHeight = node["elevation_height"].as<float>();
            }
            if (node["ground_material"]) {
                rhs.GroundMaterialGuid = node["ground_material"].as<Guid>();
            }
            if (node["wall_material"]) {
                rhs.WallMaterialGuid = node["wall_material"].as<Guid>();
            }
            return true;
        }
    };
}
