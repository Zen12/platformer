#pragma once

#include "spine_asset.hpp"
#include "spine_asset_yaml.hpp"
#include "../../asset/loaders/asset_loader.hpp"
#include <yaml-cpp/yaml.h>

// Specialization for SpineAsset
template<>
class AssetLoader<SpineAsset> : public AssetLoaderBase<SpineAsset, AssetLoader<SpineAsset>> {
public:
    static SpineAsset LoadImpl(const std::string& path) {
        const std::string value = AssetLoader<std::string>::LoadFromPath(path);
        const YAML::Node root = YAML::Load(value);
        return root.as<SpineAsset>();
    }
};
