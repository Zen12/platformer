#pragma once

#include "grid_prefab_spawner_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<GridPrefabSpawnerSerialization>
    {
        static bool decode(const Node &node, GridPrefabSpawnerSerialization &rhs) {
            rhs.prefabId = node["prefab_id"].as<std::string>();
            rhs.gridTag = node["grid_tag"].as<std::string>();
            return true;
        }
    };
}