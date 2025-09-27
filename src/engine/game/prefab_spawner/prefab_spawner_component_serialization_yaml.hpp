#pragma once

#include "prefab_spawner_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<PrefabSpawnerSerialization>
    {
        static bool decode(const Node &node, PrefabSpawnerSerialization &rhs) {
            rhs.prefabId = node["prefab_id"].as<std::string>();
            rhs.spawnTime = node["spawn_time"].as<float>();
            rhs.maxSpawn = node["max_spawn"].as<int>();
            rhs.positions = std::vector<glm::vec3>();
            if (node["positions"]) {
                auto positions = node["positions"];

                for (const auto &position : positions) {
                    rhs.positions.push_back(position.as<glm::vec3>());
                }
            }
            return true;
        }
    };
}