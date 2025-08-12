#include "asset_loader.hpp"

std::function<std::shared_ptr<AssetTextureLoader>()> AssetLoader::_textureLoader = nullptr;
