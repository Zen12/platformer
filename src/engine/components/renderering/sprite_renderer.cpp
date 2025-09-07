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

}

void SpriteRenderer::Render() const noexcept {
    if (const auto material = _material.lock()) {
        material->ClearSprites();
        material->AddSprite(_sprite);
        material->Bind();
        _mesh.Bind();
        glDrawElements(GL_TRIANGLES, static_cast<int32_t>(_mesh.GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
    }
}
