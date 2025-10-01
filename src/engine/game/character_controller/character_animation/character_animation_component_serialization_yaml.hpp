#pragma once

#include <yaml-cpp/yaml.h>

#include "character_animation_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<CharacterAnimationComponentSerialization>
    {
        static bool decode(const Node &node, CharacterAnimationComponentSerialization &rhs)
        {
            return true;
        }
    };
}