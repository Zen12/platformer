#pragma once
#include <iostream>
#include <memory>
#include <filesystem>
#include <utility>
#include <vector>
#include <string>
#include "serialization_asset.hpp"
#include "yaml/yaml_deserializer.hpp"
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
    [[nodiscard]] T LoadAssetByGuid(const std::string& guid) const noexcept {
        const auto path = GetPathFromGuid(guid);
        return YAML::LoadFile(path).as<T>();
    }


    [[nodiscard]] std::string GetPathFromGuid(const std::string& guid) const noexcept {
#ifndef NDEBUG
        if (_assetMap.find(guid) == _assetMap.end()) {
            std::cerr << "Asset " << guid << " not found" << std::endl;
        }
#endif

        const auto& meta = _assetMap.at(guid);
        return meta.Path;
    }

    template<typename T>
    [[nodiscard]] T LoadSourceByGuid(const std::string& guid) const noexcept {
        const auto path = GetPathFromGuid(guid);
        return AssetLoader::LoadFromPath<T>(path);
    }

    void UnLoadAll() {
        _assetMap.clear();
    }
};