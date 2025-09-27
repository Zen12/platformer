#pragma once

#include "idle_character_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<IdleCharacterSerialization>
    {
        static bool decode(const Node &node, IdleCharacterSerialization &rhs)
        {
            rhs.IdleAnimation = node["idle_animation"].as<std::string>();
            return true;
        }
    };
}