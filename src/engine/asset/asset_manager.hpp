#pragma once
#include <iostream>
#include <memory>
#include <filesystem>
#include <utility>
#include <vector>
#include <string>
#include "asset_files.hpp"
#include "asset_deserializer.hpp"
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "asset_loader.hpp"

class AssetManager {
private:
    const std::string _assetPath;
    std::unordered_map<std::string, MetaAsset> _assetMap;

public:
    explicit AssetManager(std::string assetPath)
        : _assetPath(std::move(assetPath))
    {
    }

    void Init();

    template <typename T, std::enable_if_t<std::is_base_of_v<Asset, T>, int> = 0>
    [[nodiscard]] T LoadAssetByGuid(const std::string& guid) const {
#ifndef NDEBUG

        if (_assetMap.find(guid) == _assetMap.end()) {
            std::cerr << "Asset " << guid << " not found" << std::endl;
        }
#endif


        const auto& meta = _assetMap.at(guid);
        return YAML::LoadFile(meta.Path).as<T>();
    }

    template<typename T>
    [[nodiscard]] T LoadSourceByGuid(const std::string& guid) const {
#ifndef NDEBUG

        if (_assetMap.find(guid) == _assetMap.end()) {
            std::cerr << "Asset " << guid << " not found" << std::endl;
        }
#endif
        const auto& meta = _assetMap.at(guid);
        return AssetLoader::LoadFromPath<T>(meta.Path);
    }

    void UnLoadAll() {
        _assetMap.clear();
    }
};