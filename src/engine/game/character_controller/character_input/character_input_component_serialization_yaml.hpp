#pragma once
#include <yaml-cpp/yaml.h>
#include "character_input_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<CharacterInputComponentSerialization>
    {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]] CharacterInputComponentSerialization &rhs)
        {
            return true;
        }
    };
}
