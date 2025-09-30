#pragma once
#include <yaml-cpp/yaml.h>
#include "scene_asset.hpp"
#include "../entity/entity_serialization_yaml.hpp"

namespace YAML {
    template <>
    struct convert<SceneAsset>
    {
        static bool decode(const Node &node, SceneAsset &rhs)
        {
            rhs.Type = node["type"].as<std::string>();
            rhs.Entities = node["entities"].as<std::vector<EntitySerialization>>();
            return true;
        }
    };
}
