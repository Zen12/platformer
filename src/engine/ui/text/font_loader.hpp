#pragma once

#include "font.hpp"
#include "../../asset/loaders/asset_loader.hpp"
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>

// Specialization for Font
template<>
class AssetLoader<Font> : public AssetLoaderBase<Font, AssetLoader<Font>> {
public:
    static Font LoadImpl(const std::string& path);
};
