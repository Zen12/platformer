#pragma once
#include "loaders/asset_loader.hpp"
#include "example_asset.hpp"
#include <sstream>

// AssetLoader specialization for ExampleAsset
// Loads a simple text file where:
//   Line 1: asset name
//   Remaining lines: entries
//
// Usage: auto asset = AssetLoader<ExampleAsset>::LoadFromPath("path/to/file.txt");
// Or via AssetManager: auto asset = assetManager->LoadSourceByGuid<ExampleAsset>(guid);
template<>
class AssetLoader<ExampleAsset> : public AssetLoaderBase<ExampleAsset, AssetLoader<ExampleAsset>> {
public:
    static void Init() {
        // Setup dependencies if needed (e.g., factory lambdas)
        // For this example, no setup is required
    }

    static ExampleAsset LoadImpl(const std::string& path) {
        ExampleAsset asset;

        const std::string content = AssetLoader<std::string>::LoadFromPath(path);
        if (content.empty()) {
            return asset;
        }

        std::istringstream stream(content);
        std::string line;

        // First line is the name
        if (std::getline(stream, line)) {
            asset.Name = line;
        }

        // Remaining lines are entries
        while (std::getline(stream, line)) {
            if (!line.empty()) {
                asset.Entries.push_back(line);
            }
        }

        return asset;
    }
};
