#pragma once
#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>
#include "glm_vec_yaml.hpp"
#include "guid/guid_yaml.hpp"
#include "meshgen_asset.hpp"

namespace YAML {
    template <>
    struct convert<MeshGenOperation> {
        static bool decode(const Node& node, MeshGenOperation& rhs) {
            rhs.Type = node["type"].as<std::string>();

            if (node["size"]) rhs.Size = node["size"].as<glm::vec3>();
            if (node["radius"]) rhs.Radius = node["radius"].as<float>();
            if (node["height"]) rhs.Height = node["height"].as<float>();
            if (node["segments"]) rhs.Segments = node["segments"].as<int>();

            if (node["translate"]) rhs.Translate = node["translate"].as<glm::vec3>();
            if (node["rotate"]) rhs.Rotate = node["rotate"].as<glm::vec3>();
            if (node["scale"]) rhs.Scale = node["scale"].as<glm::vec3>();

            if (node["bezier_guid"]) rhs.BezierGuid = node["bezier_guid"].as<Guid>();

            if (node["operands"]) {
                for (const auto& op : node["operands"]) {
                    rhs.Operands.push_back(op.as<int>());
                }
            }
            return true;
        }
    };

    template <>
    struct convert<MeshGenAsset> {
        static bool decode(const Node& node, MeshGenAsset& rhs) {
            rhs.Name = node["name"].as<std::string>();
            for (const auto& op : node["operations"]) {
                rhs.Operations.push_back(op.as<MeshGenOperation>());
            }
            return true;
        }
    };
}
