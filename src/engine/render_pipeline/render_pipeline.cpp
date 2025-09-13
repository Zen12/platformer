#include "render_pipeline.hpp"

#include "../debug/debug.hpp"

#define DEBUG_ENGINE_MESH_RENDERER_PIPELINE 0

void RenderPipeline::RenderSprites(const float& deltaTime) const
{
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    const auto projection = _camera3d.lock()->GetProjection();
    const auto view = _cameraTransform3d.lock()->GetModel();

    for (const auto& value : _sprites)
    {

        if (const auto& renderer = value.lock())
        {
            renderer->Update(deltaTime); // move to material, it is called twice per frame, need to remove it

            const auto model = renderer->GetModel();
            renderer->SetUniformMat4("model",model);
            renderer->SetUniformMat4("view",view);
            renderer->SetUniformMat4("projection",projection);

            renderer->Render();
        }
    }
}

void RenderPipeline::RenderMeshes(const float& deltaTime) {

    std::sort(_meshRenderers.begin(), _meshRenderers.end(),
        [](const std::weak_ptr<MeshRenderer>& a, const std::weak_ptr<MeshRenderer>& b) {
            const auto sa = a.lock();
            const auto sb = b.lock();

            // Sort expired items last
            if (!sa) {
                return false;
            }
            if (!sb) {
                return true;
            }
            return sa->GetZOrder() < sb->GetZOrder();
        });

    const auto projection = _camera3d.lock()->GetProjection();
    const auto view = _cameraTransform3d.lock()->GetModel();

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

            meshRenderer->Update(deltaTime); // move to material, it is called twice per frame, need to remove it

            meshRenderer->SetUniformMat4("model", meshRenderer->GetModel());
            meshRenderer->SetUniformMat4("view", view);
            meshRenderer->SetUniformMat4("projection", projection);

            meshRenderer->Render();
        }
    }

#ifndef NDEBUG
#if DEBUG_ENGINE_MESH_RENDERER_PIPELINE
    std::cout << "END RENDERING MESH"<< std::endl;

#endif
#endif
}

void RenderPipeline::RenderParticles([[maybe_unused]] const float &deltaTime) const {

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    auto SOURCE = GL_SRC_COLOR;
    auto DEST = GL_ONE;

    glBlendFunc( SOURCE, DEST);

    const auto projection = _camera3d.lock()->GetProjection();
    const auto view = _cameraTransform3d.lock()->GetModel();

    for (const auto& value : _particles) {
        if (const auto& particle = value.lock()) {
            particle->SetUniformMat4("view", view);
            particle->SetUniformMat4("projection", projection);

            particle->Render();
        }
    }
}

glm::vec3 RenderPipeline::ScreenToWorldPoint(glm::vec2 screenPos) const {
    return ScreenToWorldPoint(glm::vec3(screenPos.x, screenPos.y, 0));
}

glm::vec3 RenderPipeline::ScreenToWorldPoint(const glm::vec3 &screenPos) const {

    if (const auto camera = _camera3d.lock()) {
        return camera->ScreenToWorldPoint(screenPos);
    }

    return glm::vec3(0.0f);
}

#ifndef NDEBUG
void RenderPipeline::RenderDebugLines() const {
    glEnable(GL_CULL_FACE);

    const auto projection = _camera3d.lock()->GetProjection();
    const auto view = _cameraTransform3d.lock()->GetModel();

    DebugLines::UpdateViewProjection(view, projection);
    DebugLines::DrawLines();
    DebugLines::Clear();
}

#endif


void RenderPipeline::Init() const noexcept {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

#ifndef NDEBUG
    DebugLines::Init();
#endif
}

void RenderPipeline::ClearFrame() const noexcept {
    glClear(GL_COLOR_BUFFER_BIT);
}

void RenderPipeline::RenderUI([[maybe_unused]] const float& deltaTime) const
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const auto projection = _uiCamera.GetProjection();

    for (const auto& value : _images)
    {
        if (const auto& image = value.lock())
        {
            image->Render(projection);
        }
    }

    for (const auto& value : _texts)
    {
        if (const auto& text = value.lock())
        {
            text->Render(projection);
        }
    }

}

void RenderPipeline::Cleanup()
{
#ifndef NDEBUG
    DebugLines::Clear();
#endif
}
