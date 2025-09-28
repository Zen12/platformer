#pragma once
#include <yaml-cpp/yaml.h>
#include "spine_collider_component_serialization.hpp"

namespace YAML
{
    template <>
    struct convert<SpineColliderSerialization>
    {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]] SpineColliderSerialization &rhs) {
            return true;
        }
    };
}