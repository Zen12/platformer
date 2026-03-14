#pragma once
#include <iostream>
#include <memory>
#include <filesystem>
#include <utility>
#include <vector>
#include <string>
#include "meta/meta_asset.hpp"
#include "meta/meta_asset_yaml.hpp"
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "loaders/asset_loader.hpp"
#include "guid.hpp"

class AssetManager {
private:
    const std::string _assetPath;
    std::unordered_map<Guid, MetaAsset> _assetMap;

public:
    explicit AssetManager(std::string assetPath)
        : _assetPath(std::move(assetPath))
    {
    }

    void Init();

    template <typename T>
    [[nodiscard]] T LoadAssetByGuid(const Guid& guid) const {
        const auto path = GetPathFromGuid(guid);
        return YAML::LoadFile(path).as<T>();
    }


    [[nodiscard]] std::string GetPathFromGuid(const Guid& guid) const {
#ifndef NDEBUG
        if (_assetMap.find(guid) == _assetMap.end()) {
            std::cerr << "Asset " << guid.ToString() << " not found" << std::endl;
        }
#endif

        const auto& meta = _assetMap.at(guid);
        return meta.Path;
    }

    template<typename T>
    [[nodiscard]] T LoadSourceByGuid(const Guid& guid) const {
        const auto path = GetPathFromGuid(guid);
        return AssetLoader<T>::LoadFromPath(path);
    }

    void UnLoadAll() {
        _assetMap.clear();
    }
};
