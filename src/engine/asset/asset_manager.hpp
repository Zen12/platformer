#include <iostream>
#include <memory>
#include <filesystem>
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
    AssetManager(const std::string &assetPath)
        : _assetPath(assetPath)
    {
    }

    void Init();

    [[nodiscard]] SceneSerialization LoadSceneByGuid(const std::string &guid);
    [[nodiscard]] MaterialComponentSerialization LoadMaterialByGuid(const std::string &guid);
    [[nodiscard]] ShaderComponentSerialization LoadShaderByGuid(const std::string &vertexGuid, const std::string &framentGuid);
    [[nodiscard]] SpriteComponentSerialization LoadSpriteByGuid(const std::string &guid);

    void UnLoad(const std::string &guid);
};