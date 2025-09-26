#pragma once

#include "show_fps_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<ShowFpsComponentSerialization>
    {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]] ShowFpsComponentSerialization &rhs)
        {
            return true;
        }
    };
}