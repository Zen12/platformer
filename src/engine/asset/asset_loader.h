#pragma once

#include "../render/shader.h"
#include "../render/sprite.h"
#include "../render/font.h"
#include <ft2build.h>
#include FT_FREETYPE_H


class AssetLoader
{
    public:
        static const Shader LoadShaderFromPath(const std::string &vertexPath, const std::string &fragPath);
        static const Sprite LoadSpriteFromPath(const std::string& path);
        static const Font LoadFontFromPath(const std::string& path);
};