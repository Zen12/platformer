#pragma once
#include <yaml-cpp/yaml.h>
#include "character_animation_controller_serialization.hpp"

namespace YAML {
    template <>
    struct convert<CharacterAnimationControllerSerialization>
    {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]] CharacterAnimationControllerSerialization &rhs)
        {
            return true;
        }
    };
}
