#pragma once
#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>
#include "glm_vec_yaml.hpp"
#include "bezier_asset.hpp"

namespace YAML {
    template <>
    struct convert<BezierAsset> {
        static bool decode(const Node& node, BezierAsset& rhs) {
            rhs.Name = node["name"].as<std::string>();
            for (const auto& pt : node["control_points"]) {
                rhs.ControlPoints.push_back(pt.as<glm::vec3>());
            }
            return true;
        }
    };
}
