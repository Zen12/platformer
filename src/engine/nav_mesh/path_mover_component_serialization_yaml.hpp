#pragma once
#include <yaml-cpp/yaml.h>
#include "path_mover_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<PathMoverComponentSerialization>
    {
        static bool decode(const Node &node, PathMoverComponentSerialization &rhs) {

            rhs.Positions = std::vector<glm::vec3>();

            for (const auto &path : node["positions"]) {
                rhs.Positions.push_back(path.as<glm::vec3>());
            }

            rhs.Speed = node["speed"].as<float>();

            return true;
        }
    };
}