#pragma once
#include <yaml-cpp/yaml.h>
#include "astar_finder_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<PathFinderSerialization>
    {
        static bool decode(const Node &node, PathFinderSerialization &rhs) {
            rhs.gridTag = node["grid_tag"].as<std::string>();
            return true;
        }
    };
}