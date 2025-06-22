#pragma once

#include "../render/shader.hpp"
#include "../render/sprite.hpp"
#include "../render/font.hpp"
#include "../system/project_asset.hpp"
#include <yaml-cpp/yaml.h>

class AssetLoader
{
public:
    static Shader LoadShaderFromPath(const std::string &vertexPath, const std::string &fragPath);
    static Sprite LoadSpriteFromPath(const std::string &path);
    static Font LoadFontFromPath(const std::string &path);
    static ProjectAsset LoadProjectAssetFromPath(const std::string &path);
};