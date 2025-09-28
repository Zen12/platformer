#pragma once
#include <yaml-cpp/yaml.h>
#include "rigidbody2d_component_serialization.hpp"

namespace YAML
{
    template <>
    struct convert<Rigidbody2dSerialization>
    {
        static bool decode(const Node &node, Rigidbody2dSerialization &rhs) {
            rhs.isDynamic = node["isDynamic"].as<bool>();
            return true;
        }
    };
}