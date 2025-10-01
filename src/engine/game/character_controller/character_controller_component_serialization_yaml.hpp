#pragma once
#include <yaml-cpp/yaml.h>
#include "character_controller_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<CharacterControllerComponentSerialization>
    {
        static bool decode(const Node &node, CharacterControllerComponentSerialization &rhs)
        {
            rhs.Damage = node["damage"].as<float>();
            return true;
        }
    };
}