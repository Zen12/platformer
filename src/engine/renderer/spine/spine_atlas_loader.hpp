#pragma once

#include "../../asset/loaders/asset_loader.hpp"
#include "../../asset/loaders/asset_texture_loader.h"
#include "spine/Atlas.h"
#include <memory>
#include <functional>
#include <fstream>
#include <stdexcept>

// Specialization for spine::Atlas*
template<>
class AssetLoader<spine::Atlas*> : public AssetLoaderBase<spine::Atlas*, AssetLoader<spine::Atlas*>> {
private:
    static std::function<std::shared_ptr<AssetTextureLoader>()> _textureLoader;

public:
    static void Init();
    static spine::Atlas* LoadImpl(const std::string& path);
};
