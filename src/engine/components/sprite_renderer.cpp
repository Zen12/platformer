#include "sprite_renderer.h"

#include <GL/glew.h>


void SpriteRenderer::SetSprite(std::weak_ptr<Sprite> sprite)
{
    _sprite = sprite;
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
        glDrawElements(GL_TRIANGLES, _mesh.GetIndicesCount(), GL_UNSIGNED_INT, 0);
    }
}