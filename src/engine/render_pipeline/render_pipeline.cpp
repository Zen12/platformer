#include "render_pipeline.h"

void RenderPipeline::RenderSprites() const
{
    const auto projection = glm::value_ptr(_camera3d.lock()->GetProjection());
    const auto view = glm::value_ptr(_cameraTransform3d.lock()->GetModel());

    for (auto value : _sprites) {

        if (auto sprite = value.lock())
        {
            sprite->Update(); // move to material

            const auto model = sprite->GetEntity().lock()->GetComponent<Transform>().lock();

            uint32_t shaderId = sprite->GetShaderId();


            unsigned int modelLock = glGetUniformLocation(shaderId, "model");
            glUniformMatrix4fv(modelLock, 1, GL_FALSE, glm::value_ptr(model->GetModel()));
    
            unsigned int viewLock = glGetUniformLocation(shaderId, "view");
            glUniformMatrix4fv(viewLock, 1, GL_FALSE, view);

            unsigned int projLock = glGetUniformLocation(shaderId, "projection");
            glUniformMatrix4fv(projLock, 1, GL_FALSE, projection);

            sprite->Render();
        }
    }
}


void RenderPipeline::RenderUI() const
{
    const auto projection = glm::value_ptr(_uiCamera.GetProjection());
    _uiShader.Use();
    glUniformMatrix4fv(glGetUniformLocation(_uiShader.GetShaderId(), "projection"), 1, GL_FALSE, projection);

    const auto width = _window.lock()->GetWidth();
    const auto height = _window.lock()->GetHeight();


    for (auto value : _texts) {
        if (auto text = value.lock())
        {
            text->Render(_uiShader, width, height);
        }
    }
}


void RenderPipeline::Cleanup()
{

}
