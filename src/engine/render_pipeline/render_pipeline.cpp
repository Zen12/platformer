#include "render_pipeline.hpp"

#define DEBUG_ENGINE_MESH_RENDERER_PIPELINE 0

void RenderPipeline::RenderSprites(const float& deltaTime) const
{
    const auto projectionMat = _camera3d.lock()->GetProjection();
    const auto viewMat = _cameraTransform3d.lock()->GetModel();

    const auto projection = glm::value_ptr(projectionMat);
    const auto view = glm::value_ptr(viewMat);

    for (const auto& value : _sprites)
    {

        if (const auto& sprite = value.lock())
        {
            sprite->Update(deltaTime); // move to material

            const auto model = sprite->GetEntity().lock()->GetComponent<Transform>().lock();

            const auto shaderId = sprite->GetShaderId();

            const auto modelLock = glGetUniformLocation(shaderId, "model");
            glUniformMatrix4fv(modelLock, 1, GL_FALSE, glm::value_ptr(model->GetModel()));

            const auto viewLock = glGetUniformLocation(shaderId, "view");
            glUniformMatrix4fv(viewLock, 1, GL_FALSE, view);

            const auto projLock = glGetUniformLocation(shaderId, "projection");
            glUniformMatrix4fv(projLock, 1, GL_FALSE, projection);

            sprite->Render();
        }
    }
}

void RenderPipeline::RenderLines(const float& deltaTime) const {

    const auto projectionMat = _camera3d.lock()->GetProjection();
    const auto viewMat = _cameraTransform3d.lock()->GetModel();

    const auto projection = glm::value_ptr(projectionMat);
    const auto view = glm::value_ptr(viewMat);

    for (const auto& value : _lines)
    {

        if (const auto& line = value.lock())
        {
            line->Update(deltaTime); // move to material

            const auto model = line->GetEntity().lock()->GetComponent<Transform>().lock();

            const auto shaderId = line->GetShaderId();

            const auto modelLock = glGetUniformLocation(shaderId, "model");
            glUniformMatrix4fv(modelLock, 1, GL_FALSE, glm::value_ptr(model->GetModel()));

            const auto viewLock = glGetUniformLocation(shaderId, "view");
            glUniformMatrix4fv(viewLock, 1, GL_FALSE, view);

            const auto projLock = glGetUniformLocation(shaderId, "projection");
            glUniformMatrix4fv(projLock, 1, GL_FALSE, projection);

            line->Render();
        }
    }

}

void RenderPipeline::RenderMeshes(const float& deltaTime) {

    std::sort(_meshRenderers.begin(), _meshRenderers.end(),
        [](const std::weak_ptr<MeshRenderer>& a, const std::weak_ptr<MeshRenderer>& b) {
            auto sa = a.lock();
            auto sb = b.lock();

            // Sort expired items last
            if (!sa) {
                return false;
            }
            if (!sb) {
                return true;
            }
            return sa->GetZOrder() < sb->GetZOrder();
        });

    const auto projectionMat = _camera3d.lock()->GetProjection();
    const auto viewMat = _cameraTransform3d.lock()->GetModel();

    const auto projection = glm::value_ptr(projectionMat);
    const auto view = glm::value_ptr(viewMat);

#ifndef NDEBUG
#if DEBUG_ENGINE_MESH_RENDERER_PIPELINE
      std::cout << "START RENDERING MESH"<< std::endl;

#endif
#endif

    for (const auto& value : _meshRenderers)
    {

        if (const auto& meshRenderer = value.lock())
        {

#ifndef NDEBUG
#if DEBUG_ENGINE_MESH_RENDERER_PIPELINE
            std::cout << meshRenderer->GetEntity().lock()->GetId() << std::endl;
#endif
#endif

            meshRenderer->Update(deltaTime); // move to material

            const auto model = meshRenderer->GetEntity().lock()->GetComponent<Transform>().lock();

            const auto shaderId = meshRenderer->GetShaderId();

            const auto modelLock = glGetUniformLocation(shaderId, "model");
            glUniformMatrix4fv(modelLock, 1, GL_FALSE, glm::value_ptr(model->GetModel()));

            const auto viewLock = glGetUniformLocation(shaderId, "view");
            glUniformMatrix4fv(viewLock, 1, GL_FALSE, view);

            const auto projLock = glGetUniformLocation(shaderId, "projection");
            glUniformMatrix4fv(projLock, 1, GL_FALSE, projection);

            meshRenderer->Render();
        }
    }

#ifndef NDEBUG
#if DEBUG_ENGINE_MESH_RENDERER_PIPELINE
    std::cout << "END RENDERING MESH"<< std::endl;

#endif
#endif
}

void RenderPipeline::Init() const noexcept {
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void RenderPipeline::ClearFrame() const noexcept {
    glClear(GL_COLOR_BUFFER_BIT);
}

void RenderPipeline::RenderUI(const float& deltaTime) const
{
    const auto projection = _uiCamera.GetProjection();

    for (const auto& value : _texts)
    {
        if (const auto& text = value.lock())
        {
            text->Update(deltaTime);
            text->Render(projection);
        }
    }

    for (const auto& value : _images)
    {
        if (const auto& image = value.lock())
        {
            image->Update(deltaTime);
            image->Render(projection);
        }
    }
}

void RenderPipeline::Cleanup()
{
}
