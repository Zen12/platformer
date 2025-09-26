#pragma once

#include "text_renderer_component_serialization.hpp"
#include "glm/vec3.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<UiTextComponentSerialization>
    {
        static bool decode(const YAML::Node &node, UiTextComponentSerialization &rhs)
        {
            rhs.MaterialGUID = node["material"].as<std::string>();
            rhs.Text = node["text"].as<std::string>();
            rhs.Color = node["color"].as<glm::vec3>();
            rhs.FontSize = node["font_size"].as<float>();
            return true;
        }
    };
}
