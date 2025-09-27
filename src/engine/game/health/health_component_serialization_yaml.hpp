#pragma once
#include <yaml-cpp/yaml.h>
#include "health_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<HealthComponentSerialization>
    {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]] HealthComponentSerialization &rhs)
        {
            rhs.Health = node["health"].as<float>();
            return true;
        }
    };
}