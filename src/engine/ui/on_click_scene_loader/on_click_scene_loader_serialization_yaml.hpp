#pragma once

#include "on_click_scene_loader_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<OnClickSceneLoaderSerialization>
    {
        static bool decode(const Node &node, OnClickSceneLoaderSerialization &rhs)
        {
            rhs.SceneGuid = node["scene_guid"].as<std::string>();
            return true;
        }
    };
}