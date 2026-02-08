#pragma once
#include "load_scene_action_serialization.hpp"
#include "yaml-cpp/node/node.h"
#include "../system/guid_yaml.hpp"

namespace YAML {
    template <>
    struct convert<LoadSceneActionSerialization>
    {
        static bool decode(const Node &node, LoadSceneActionSerialization &rhs)
        {
            rhs.SceneGuid = node["scene_guid"].as<Guid>();
            return true;
        }
    };
}
