#pragma once
#include "camera_controller_component_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<CameraControllerComponentSerialization> {
        static bool decode(const Node &node, CameraControllerComponentSerialization &rhs) {
            if (node["move_speed"]) {
                rhs.MoveSpeed = node["move_speed"].as<float>();
            }
            if (node["mouse_sensitivity"]) {
                rhs.MouseSensitivity = node["mouse_sensitivity"].as<float>();
            }
            return true;
        }
    };
}
