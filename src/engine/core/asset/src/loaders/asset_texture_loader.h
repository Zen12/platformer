#pragma once

#include "GL/glew.h"
#include <iostream>
#include <string>



class AssetTextureLoader {
public:

    unsigned int texture_load(const char *file_path, bool flip_vertically = true, int *width = nullptr, int *height = nullptr);

    void texture_dispose(unsigned int texture) {
        glDeleteTextures(1, &texture);
    }
};
