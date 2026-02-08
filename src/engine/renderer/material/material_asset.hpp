#pragma once
#include "../../system/guid.hpp"

struct MaterialAsset
{
    Guid VertexShaderGuid{};
    Guid FragmentShaderGuid{};
    Guid Font{};
    Guid Image{};
    int BlendMode = 0;
    bool IsCulling = false;
    bool IsDepthTest = false;
    bool IsDepthWrite = true;
};
