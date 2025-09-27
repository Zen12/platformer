#pragma once

#include "grid_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<GridSerialization> {
        static bool decode(const Node &node, GridSerialization &rhs) {
            rhs.spawnOffset = node["spawn_offset"].as<glm::vec3>();
            rhs.spawnStep = node["spawn_step"].as<glm::vec3>();
            rhs.grid = std::vector<std::vector<int>>();
            if (node["grid"]) {
                const auto grid = node["grid"];

                for (const auto &position : grid) {
                    std::vector<int> vec;
                    for (const auto &row : position) {
                        vec.push_back(row.as<int>());
                    }
                    rhs.grid.push_back(vec);
                }
            }
            return true;
        }
    };
}