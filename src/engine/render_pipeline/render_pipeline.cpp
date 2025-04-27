#include "render_pipeline.h"

void RenderPipeline::RenderSprites() const
{
    for (auto value : _sprites) {

        if (auto sprite = value.lock())
        {
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
