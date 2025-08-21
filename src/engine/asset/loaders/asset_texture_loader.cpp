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

    GLenum format = GL_RGBA;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return texture;
}

uint32_t AssetTextureLoader::load_sprite(const std::string &path) {

    uint32_t _textureId = 0;
    int _x, _y, _channels;

    glGenTextures(1, &_textureId);
    glBindTexture(GL_TEXTURE_2D, _textureId);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    unsigned char *dataPtr = stbi_load(path.c_str(), &_x, &_y, &_channels, 0);

    if (dataPtr == nullptr)
    {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return 0;
    }

    switch (_channels)
    {
        case 3:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _x, _y, 0, GL_RGB, GL_UNSIGNED_BYTE, dataPtr);
            break;
        case 4:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _x, _y, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataPtr);
            break;
        default:
            std::cerr << "No support for channel: " << _channels << std::endl;
            break;
    }

    stbi_image_free(dataPtr);
    return _textureId;
}
