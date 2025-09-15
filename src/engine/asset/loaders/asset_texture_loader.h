#pragma once

#include "spine/TextureLoader.h"
#include "spine/Atlas.h"
#include "GL/glew.h"
#include <iostream>
#include <string>



class AssetTextureLoader final : public spine::TextureLoader {
public:

    unsigned int texture_load(const char *file_path);

    void texture_dispose(unsigned int texture) {
        glDeleteTextures(1, &texture);
    }

    void load(spine::AtlasPage &page, const spine::String &path) override {
        page.texture = (void *) (uintptr_t) texture_load(path.buffer());
    }


    void unload(void *texture) override {
        texture_dispose((unsigned int) (uintptr_t) texture);
    }
};
