#include "texture_asset_loader.hpp"

// Static member initialization
std::function<std::shared_ptr<AssetTextureLoader>()> AssetLoader<Texture>::_textureLoader = nullptr;

void AssetLoader<Texture>::Init() {
    AssetLoader::_textureLoader = []() {
        return std::make_shared<AssetTextureLoader>();
    };
}

Texture AssetLoader<Texture>::LoadImpl(const std::string& path) {
    auto loader = _textureLoader();
    return Texture(loader->texture_load(path.c_str()));
}
