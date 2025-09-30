#pragma once
#include <yaml-cpp/yaml.h>
#include "prefab_asset.hpp"
#include "../entity/entity_serialization_yaml.hpp"

namespace YAML {
    template <>
    struct convert<PrefabAsset>
    {
        static bool decode(const Node &node, PrefabAsset &rhs)
        {
            rhs.Obj = node.as<EntitySerialization>();
            return true;
        }
    };
}