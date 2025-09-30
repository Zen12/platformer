#pragma once

namespace YAML {
    template <>
    struct convert<MaterialAsset>
    {
        static bool decode(const Node &node, MaterialAsset &rhs)
        {
            rhs.BlendMode = node["blend_mode"].as<int>();
            rhs.IsCulling = node["is_culling"].as<bool>();

#ifdef __EMSCRIPTEN__
            if (const auto shaderNode = node["shader_gles"]) {
                rhs.VertexShaderGuid = shaderNode["vertex"].as<std::string>();
                rhs.FragmentShaderGuid = shaderNode["fragment"].as<std::string>();
            }
#else
            if (const auto shaderNode = node["shader_opengl"]) {
                rhs.VertexShaderGuid = shaderNode["vertex"].as<std::string>();
                rhs.FragmentShaderGuid = shaderNode["fragment"].as<std::string>();
            }
#endif


            if (node["font"] && node["font"].IsScalar()) {
                rhs.Font = node["font"].as<std::string>();
            }

            if (node["image"] && node["image"].IsScalar()) {
                rhs.Image = node["image"].as<std::string>();
            }

            return true;
        }
    };
}