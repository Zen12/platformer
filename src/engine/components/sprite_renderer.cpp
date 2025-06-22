#include "sprite_renderer.hpp"

#include <GL/glew.h>

#include <utility>

void SpriteRenderer::SetSprite(std::weak_ptr<Sprite> sprite)
{
    _sprite = std::move(sprite);
}

void SpriteRenderer::Update() const
{
    if (auto sprite = _sprite.lock())
    {
        sprite->Bind();
        _shader.Use();
        _mesh.Bind();
    }
}

void SpriteRenderer::Render() const
{
    if (auto sprite = _sprite.lock())
    {
        glDrawElements(GL_TRIANGLES, static_cast<int32_t>(_mesh.GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
    }
}