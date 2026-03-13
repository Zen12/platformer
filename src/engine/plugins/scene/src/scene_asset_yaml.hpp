#pragma once
#include <yaml-cpp/yaml.h>
#include "scene_asset.hpp"
#include "entity/entity_serialization_yaml.hpp"
#include "guid_yaml.hpp"
#include "navmesh_config_yaml.hpp"

namespace YAML {
    template <>
    struct convert<SkyboxConfig>
    {
        static bool decode(const Node &node, SkyboxConfig &rhs)
        {
            if (node["material"]) {
                rhs.MaterialGuid = node["material"].as<Guid>();
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
