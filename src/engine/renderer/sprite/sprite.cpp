#include "sprite.hpp"


Sprite::Sprite(const uint32_t &textureId)
: _textureId(textureId)
{

}

void Sprite::Bind() const noexcept
{
    glBindTexture(GL_TEXTURE_2D, _textureId);
}

[[nodiscard]] uint32_t Sprite::GetTextureId() const noexcept {
    return _textureId;
}

Sprite::~Sprite()
{
    glDeleteTextures(1, &_textureId);
}

Sprite::Sprite(Sprite&& other) noexcept
    : _textureId(other._textureId)
{
    other._textureId = 0;
}

Sprite& Sprite::operator=(Sprite&& other) noexcept
{
    if (this != &other)
    {
        glDeleteTextures(1, &_textureId);
        _textureId = other._textureId;
        other._textureId = 0;
    }
    return *this;
}
