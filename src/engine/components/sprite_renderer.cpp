#include "sprite_renderer.h"

#include <GL/glew.h>


void SpriteRenderer::SetSprite(std::weak_ptr<Sprite> sprite)
{
    _sprite = sprite;
}

void SpriteRenderer::SetCamera(std::weak_ptr<CameraComponent> camera)
{
    _cameraComponent = camera;
}

void SpriteRenderer::Update() const 
{

}

void SpriteRenderer::Render() const 
{
    
    if (auto sprite = _sprite.lock()) 
    {
        if ( auto camera = _cameraComponent.lock())
        {

        if (auto cameraTransform = camera->GetEntity().lock()->GetComponent<Transform>().lock())
        {
            
            if (auto tr = GetEntity().lock()->GetComponent<Transform>().lock())
            {

            sprite->Bind();
            _shader.Use();
            _mesh.Bind();
    
            uint32_t shaderId = GetShaderId();


            unsigned int modelLock = glGetUniformLocation(shaderId, "model");
            glUniformMatrix4fv(modelLock, 1, GL_FALSE, glm::value_ptr(tr->GetModel()));
    
            unsigned int viewLock = glGetUniformLocation(shaderId, "view");
            glUniformMatrix4fv(viewLock, 1, GL_FALSE, glm::value_ptr(cameraTransform->GetModel()));

            unsigned int projLock = glGetUniformLocation(shaderId, "projection");
            glUniformMatrix4fv(projLock, 1, GL_FALSE, glm::value_ptr(camera->GetProjection()));
            
            glDrawElements(GL_TRIANGLES, _mesh.GetIndicesCount(), GL_UNSIGNED_INT, 0);
            }
        }
    }

    }
}