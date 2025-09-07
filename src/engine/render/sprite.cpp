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
