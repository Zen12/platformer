#include "sprite_asset_loader.hpp"

// Static member initialization
std::function<std::shared_ptr<AssetTextureLoader>()> AssetLoader<Sprite>::_textureLoader = nullptr;

void AssetLoader<Sprite>::Init() {
    AssetLoader::_textureLoader = []() {
        return std::make_shared<AssetTextureLoader>();
    };
}

Sprite AssetLoader<Sprite>::LoadImpl(const std::string& path) {
    auto loader = _textureLoader();
    return Sprite(loader->texture_load(path.c_str()));
}
