#include "asset_loader.h"
#include "../system/file_loader.h"

const Shader AssetLoader::LoadShaderFromPath(const std::string &vertexPath, const std::string &fragPath)
{
    std::string vertexShaderSource = FileLoader::LoadFile(vertexPath);
    std::string fragmentShaderSource = FileLoader::LoadFile(fragPath);

    return Shader(vertexShaderSource, fragmentShaderSource);
}

const Sprite AssetLoader::LoadSpriteFromPath(const std::string &path)
{
    return Sprite((ASSETS_PATH + path).c_str());
}
