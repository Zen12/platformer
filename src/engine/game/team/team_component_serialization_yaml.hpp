#pragma once

#include "team_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<TeamSerialization>
    {
        static bool decode(const Node &node, TeamSerialization &rhs)
        {
            rhs.Team = node["team"].as<int>();
            return true;
        }
    };
}