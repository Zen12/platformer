#pragma once
#include <yaml-cpp/yaml.h>
#include "sprite_renderer_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<SpriteRenderComponentSerialization>
    {
        static bool decode(const Node &node, SpriteRenderComponentSerialization &rhs) {
            rhs.MaterialGuid = node["material"].as<std::string>();
            rhs.SpriteGuid = node["image"].as<std::string>();
            return true;
        }
    };
}