#pragma once
#include "top_down_camera_component_serialization.hpp"
#include "../../math/glm_vec_yaml.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<TopDownCameraComponentSerialization> {
        static bool decode(const Node& node, TopDownCameraComponentSerialization& rhs) {
            rhs.TargetTag = node["target_tag"] ? node["target_tag"].as<std::string>() : "";
            rhs.OffsetPosition = node["offset_position"] ? node["offset_position"].as<glm::vec3>() : glm::vec3(0.0f, 10.0f, 10.0f);
            rhs.OffsetRotation = node["offset_rotation"] ? node["offset_rotation"].as<glm::vec3>() : glm::vec3(-45.0f, 0.0f, 0.0f);
            rhs.MaxLookAhead = node["max_look_ahead"] ? node["max_look_ahead"].as<float>() : 5.0f;
            rhs.SmoothSpeed = node["smooth"] ? node["smooth"].as<float>() : 5.0f;
            rhs.MaxMoveSpeed = node["max_move_speed"] ? node["max_move_speed"].as<float>() : 15.0f;
            return true;
        }
    };
}
