#include "sprite_renderer.hpp"

#include <GL/glew.h>

#include <utility>

void SpriteRenderer::SetSprite(std::weak_ptr<Sprite> sprite) noexcept {
    _sprite = std::move(sprite);
}

void SpriteRenderer::SetMaterial(std::weak_ptr<Material> material) noexcept {
    _material = std::move(material);
}

void SpriteRenderer::Update([[maybe_unused]] const float& deltaTime)
{
    if ( const auto sprite = _sprite.lock())
    {
        sprite->Bind();
        if (const auto material = _material.lock()) {
            material->Bind();
        }
        _mesh.Bind();
    }
}

void SpriteRenderer::Render() const noexcept
{
    if (auto sprite = _sprite.lock())
    {
        glDrawElements(GL_TRIANGLES, static_cast<int32_t>(_mesh.GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
    }
}
