#pragma once

#include "sprite.hpp"
#include "../../asset/loaders/asset_loader.hpp"
#include "../../asset/loaders/asset_texture_loader.h"
#include <memory>
#include <functional>

// Specialization for Sprite
template<>
class AssetLoader<Sprite> : public AssetLoaderBase<Sprite, AssetLoader<Sprite>> {
private:
    static std::function<std::shared_ptr<AssetTextureLoader>()> _textureLoader;

public:
    static void Init();
    static Sprite LoadImpl(const std::string& path);
};
