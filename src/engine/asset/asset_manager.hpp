#pragma once
#include <iostream>
#include <memory>
#include <filesystem>
#include <utility>
#include <vector>
#include <string>
#include "asset_meta.hpp"
#include "asset_deserializer.hpp"
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "../system/file_loader.hpp"

class AssetManager
{
private:
    const std::string _assetPath;
    std::unordered_map<std::string, AssetMeta> _assetMap;

public:
    AssetManager(std::string assetPath)
        : _assetPath(std::move(assetPath))
    {
    }

    void Init();

    [[nodiscard]] SceneSerialization LoadSceneByGuid(const std::string &guid) const;
    [[nodiscard]] MaterialComponentSerialization LoadMaterialByGuid(const std::string &guid) const;
    [[nodiscard]] ShaderComponentSerialization LoadShaderByGuid(const std::string &vertexGuid, const std::string &fragmentGuid) const;
    [[nodiscard]] SpriteComponentSerialization LoadSpriteByGuid(const std::string &guid) const;

    void UnLoad(const std::string &guid) const;
};