#pragma once

#include "project_asset.hpp"
#include "project_asset_yaml.hpp"
#include "../asset/loaders/asset_loader.hpp"
#include <yaml-cpp/yaml.h>

// Specialization for ProjectAsset
template<>
class AssetLoader<ProjectAsset> : public AssetLoaderBase<ProjectAsset, AssetLoader<ProjectAsset>> {
public:
    static ProjectAsset LoadImpl(const std::string& path) {
        const std::string value = AssetLoader<std::string>::LoadFromPath(path);
        const YAML::Node root = YAML::Load(value);
        return root.as<ProjectAsset>();
    }
};
