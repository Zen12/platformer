#pragma once

#include "image_renderer_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<UiImageComponentSerialization>
    {
        static bool decode(const Node &node, UiImageComponentSerialization &rhs) {
            rhs.MaterialGuid = node["material"].as<std::string>();
            rhs.SpriteGuid = node["image"].as<std::string>();
            if (node["fill_amount"])
                rhs.FillAmount = node["fill_amount"].as<float>();
            return true;
        }
    };
}