#include "render_pipeline.hpp"

void RenderPipeline::RenderSprites() const
{
    const auto projection = glm::value_ptr(_camera3d.lock()->GetProjection());
    const auto view = glm::value_ptr(_cameraTransform3d.lock()->GetModel());

    for (const auto& value : _sprites)
    {

        if (const auto& sprite = value.lock())
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

void RenderPipeline::Init() const {
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void RenderPipeline::ClearFrame() const {
    glClear(GL_COLOR_BUFFER_BIT);
}

void RenderPipeline::RenderUI() const
{
    const auto projection = _uiCamera.GetProjection();

    for (const auto& value : _texts)
    {
        if (const auto& text = value.lock())
        {
            text->Update();
            text->Render(projection);
        }
    }

    for (const auto& value : _images)
    {
        if (const auto& image = value.lock())
        {
            image->Update();
            image->Render(projection);
        }
    }
}

void RenderPipeline::Cleanup()
{
}
