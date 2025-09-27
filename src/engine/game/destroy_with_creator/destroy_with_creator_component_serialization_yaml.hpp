#pragma once

#include "destroy_with_creator_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<DestroyWithCreatorComponentSerialization>
    {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]]  DestroyWithCreatorComponentSerialization &rhs)
        {
            return true;
        }
    };
}