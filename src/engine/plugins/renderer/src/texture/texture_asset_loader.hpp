#pragma once

#include "texture.hpp"
#include "loaders/asset_loader.hpp"
#include "asset_texture_loader.hpp"
#include <memory>
#include <functional>

// Specialization for Sprite
template<>
class AssetLoader<Texture> : public AssetLoaderBase<Texture, AssetLoader<Texture>> {
private:
    static std::function<std::shared_ptr<AssetTextureLoader>()> _textureLoader;

public:
    static void Init();
    static Texture LoadImpl(const std::string& path);
};
