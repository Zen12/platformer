#pragma once
#include <yaml-cpp/yaml.h>
#include "bone_attachment_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<BoneAttachmentComponentSerialization> {
        static bool decode(const Node &node, BoneAttachmentComponentSerialization &rhs) {
            rhs.ParentTag = node["parent_tag"].as<std::string>();
            rhs.BoneName = node["bone_name"].as<std::string>();
            if (node["position_offset"]) {
                auto off = node["position_offset"];
                rhs.PositionOffset = glm::vec3(off[0].as<float>(), off[1].as<float>(), off[2].as<float>());
            }
            if (node["rotation_offset"]) {
                auto rot = node["rotation_offset"];
                rhs.RotationOffset = glm::vec3(rot[0].as<float>(), rot[1].as<float>(), rot[2].as<float>());
            }
            return true;
        }
    };
}
