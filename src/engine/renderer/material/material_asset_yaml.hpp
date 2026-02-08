#pragma once

#include "material_asset.hpp"
#include "yaml-cpp/node/node.h"
#include "../../system/guid_yaml.hpp"

namespace YAML {
    template <>
    struct convert<MaterialAsset>
    {
        static bool decode(const Node &node, MaterialAsset &rhs)
        {
            rhs.BlendMode = node["blend_mode"].as<int>();
            rhs.IsCulling = node["is_culling"].as<bool>();

            if (node["is_depth_test"]) {
                rhs.IsDepthTest = node["is_depth_test"].as<bool>();
            }

            if (node["is_depth_write"]) {
                rhs.IsDepthWrite = node["is_depth_write"].as<bool>();
            }

#ifdef __EMSCRIPTEN__
            if (const auto shaderNode = node["shader_gles"]) {
                rhs.VertexShaderGuid = shaderNode["vertex"].as<Guid>();
                rhs.FragmentShaderGuid = shaderNode["fragment"].as<Guid>();
            }
#else
            if (const auto shaderNode = node["shader_opengl"]) {
                rhs.VertexShaderGuid = shaderNode["vertex"].as<Guid>();
                rhs.FragmentShaderGuid = shaderNode["fragment"].as<Guid>();
            }
#endif


            if (node["font"] && node["font"].IsScalar()) {
                rhs.Font = node["font"].as<Guid>();
            }

            if (node["image"] && node["image"].IsScalar()) {
                rhs.Image = node["image"].as<Guid>();
            }

            return true;
        }
    };
}
