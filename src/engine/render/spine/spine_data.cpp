#include "spine_data.hpp"



/// Set the default extension used for memory allocations and file I/O
spine::SpineExtension *spine::getDefaultExtension() {
    return new spine::DefaultSpineExtension();
}

unsigned int texture_load(const char *file_path) {
    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(file_path, &width, &height, &nrChannels, 0);
    if (!data) {
        printf("Failed to load texture\n");
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

void GlTextureLoader::load(spine::AtlasPage &page, const spine::String &path) {
    page.texture = (void *) (uintptr_t) texture_load(path.buffer());
}

void texture_dispose(unsigned int texture) {
    glDeleteTextures(1, &texture);
}

void GlTextureLoader::unload(void *texture) {
    texture_dispose((unsigned int) (uintptr_t) texture);
}