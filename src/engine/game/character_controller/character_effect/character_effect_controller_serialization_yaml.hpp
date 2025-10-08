#pragma once
#include <yaml-cpp/yaml.h>
#include "character_effect_controller_serialization.hpp"

namespace YAML {
    template <>
    struct convert<CharacterEffectControllerSerialization>
    {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]] CharacterEffectControllerSerialization &rhs)
        {
            return true;
        }
    };
}
