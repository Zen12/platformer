#pragma once


struct MaterialAsset
{
    std::string VertexShaderGuid{};
    std::string FragmentShaderGuid{};
    std::string Font{};
    std::string Image{};
    int BlendMode = 0;
    bool IsCulling = false;
    bool IsDepthTest = false;
    bool IsDepthWrite = true;
};