#pragma once

#include "camera_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<CameraComponentSerialization>
    {
        static bool decode(const Node &node, CameraComponentSerialization &rhs)
        {
            if (node["perspectiveAspect"])
                rhs.perspectiveAspect = node["perspectiveAspect"].as<float>();
            if (node["perspectiveNearPlane"])
                rhs.perspectiveNearPlane = node["perspectiveNearPlane"].as<float>();
            if (node["perspectiveFarPlane"])
                rhs.perspectiveFarPlane = node["perspectiveFarPlane"].as<float>();
            if (node["orthographicAspect"])
                rhs.orthographicAspect = node["orthographicAspect"].as<float>();

            rhs.isPerspective = node["isPerspective"].as<bool>();
            if (node["yDepthFactor"])
                rhs.yDepthFactor = node["yDepthFactor"].as<float>();
            return true;
        }
    };
}