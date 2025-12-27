#pragma once
#include "ozz_ik_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<OzzIKComponentSerialization> {
        static bool decode(const Node &node, OzzIKComponentSerialization &rhs) {
            if (!node.IsMap()) {
                return false;
            }

            if (node["ik_type"]) {
                rhs.Type = node["ik_type"].as<std::string>();
            } else if (node["type"]) {
                // Backwards compatibility
                rhs.Type = node["type"].as<std::string>();
            }
            if (node["enabled"]) {
                rhs.Enabled = node["enabled"].as<bool>();
            }
            if (node["weight"]) {
                rhs.Weight = node["weight"].as<float>();
            }
            if (node["start_bone"]) {
                rhs.StartBoneName = node["start_bone"].as<std::string>();
            }
            if (node["mid_bone"]) {
                rhs.MidBoneName = node["mid_bone"].as<std::string>();
            }
            if (node["end_bone"]) {
                rhs.EndBoneName = node["end_bone"].as<std::string>();
            }
            if (node["target"]) {
                auto target = node["target"];
                rhs.Target = glm::vec3(
                    target[0].as<float>(),
                    target[1].as<float>(),
                    target[2].as<float>()
                );
            }
            if (node["pole_vector"]) {
                auto pole = node["pole_vector"];
                rhs.PoleVector = glm::vec3(
                    pole[0].as<float>(),
                    pole[1].as<float>(),
                    pole[2].as<float>()
                );
            }
            if (node["aim_axis"]) {
                auto aim = node["aim_axis"];
                rhs.AimAxis = glm::vec3(
                    aim[0].as<float>(),
                    aim[1].as<float>(),
                    aim[2].as<float>()
                );
            }
            if (node["up_axis"]) {
                auto up = node["up_axis"];
                rhs.UpAxis = glm::vec3(
                    up[0].as<float>(),
                    up[1].as<float>(),
                    up[2].as<float>()
                );
            }

            return true;
        }
    };
}
