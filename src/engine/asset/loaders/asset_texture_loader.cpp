#include "asset_texture_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int AssetTextureLoader::texture_load(const char *file_path) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(file_path, &width, &height, &nrChannels, 0);
    if (!data) {
        printf("Failed to load texture at path: %s\n", file_path);
        return 0;
    }

    GLint internalFormat = GL_RGBA8;
    GLint format = GL_RGBA;
    if (nrChannels == 1) {
        internalFormat = GL_R8;
        format = GL_RED;
    } else if (nrChannels == 3) {
        internalFormat = GL_RGB8;
        format = GL_RGB;
    } else if (nrChannels == 4) {
        internalFormat = GL_RGBA8;
        format = GL_RGBA;
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return texture;
}
