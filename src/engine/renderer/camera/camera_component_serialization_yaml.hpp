#pragma once

#include "camera_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<CameraComponentSerialization>
    {
        static bool decode(const Node &node, CameraComponentSerialization &rhs)
        {
            rhs.aspectPower = node["aspectPower"].as<float>();
            rhs.isPerspective = node["isPerspective"].as<bool>();
            return true;
        }
    };
}