#pragma once

#include "../render/shader.hpp"
#include "../render/sprite.hpp"
#include "../render/font.hpp"
#include "../system/project_asset.hpp"
#include <yaml-cpp/yaml.h>

class AssetLoader
{
public:
    static const Shader LoadShaderFromPath(const std::string &vertexPath, const std::string &fragPath);
    static const Sprite LoadSpriteFromPath(const std::string &path);
    static const Font LoadFontFromPath(const std::string &path);
    static ProjectAsset LoadProjectAssetFromPath(const std::string &path);
};