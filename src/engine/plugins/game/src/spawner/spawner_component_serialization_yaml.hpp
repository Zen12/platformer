#pragma once
#include <yaml-cpp/yaml.h>
#include "spawner_component_serialization.hpp"
#include "guid_yaml.hpp"
#include "glm_vec_yaml.hpp"

namespace YAML {
    template <>
    struct convert<SpawnerComponentSerialization> {
        static bool decode(const Node &node, SpawnerComponentSerialization &rhs) {
            if (node["prefab_guid"]) {
                rhs.PrefabGuid = node["prefab_guid"].as<Guid>();
            }
            if (node["spawn_count"]) {
                rhs.SpawnCount = node["spawn_count"].as<int>();
            }
            if (node["spawn_positions"]) {
                rhs.SpawnPositions = node["spawn_positions"].as<std::vector<glm::vec3>>();
            }
            if (node["spawn_on_navmesh"]) {
                rhs.SpawnOnNavmesh = node["spawn_on_navmesh"].as<bool>();
            }
            if (node["spawn_on_all_cells"]) {
                rhs.SpawnOnAllCells = node["spawn_on_all_cells"].as<bool>();
            }
            if (node["elevation_height"]) {
                rhs.ElevationHeight = node["elevation_height"].as<float>();
            }
            if (node["y_offset"]) {
                rhs.YOffset = node["y_offset"].as<float>();
            }
            return true;
        }
    };
}
