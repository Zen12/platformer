#include <iostream>
#include <memory>
#include <filesystem>
#include <vector>
#include <string>
#include "asset_meta.hpp"
#include "asset_deserializer.hpp"
#include <fstream>
#include <yaml-cpp/yaml.h>

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

    void LoadSceneByGuid(const std::string &guid);
    void UnLoad(const std::string &guid);
};