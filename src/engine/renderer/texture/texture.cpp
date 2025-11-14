#include "texture.hpp"


Texture::Texture(const uint32_t &textureId)
: _textureId(textureId)
{

}

void Texture::Bind() const noexcept
{
    glBindTexture(GL_TEXTURE_2D, _textureId);
}

[[nodiscard]] uint32_t Texture::GetTextureId() const noexcept {
    return _textureId;
}

Texture::~Texture()
{
    glDeleteTextures(1, &_textureId);
}

Texture::Texture(Texture&& other) noexcept
    : _textureId(other._textureId)
{
    other._textureId = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other)
    {
        glDeleteTextures(1, &_textureId);
        _textureId = other._textureId;
        other._textureId = 0;
    }
    return *this;
}
