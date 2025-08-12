#include "render_pipeline.hpp"

#include "../debug/debug.hpp"

#define DEBUG_ENGINE_MESH_RENDERER_PIPELINE 0

void RenderPipeline::RenderSprites(const float& deltaTime) const
{

    glEnable(GL_CULL_FACE);

    const auto projection = _camera3d.lock()->GetProjection();
    const auto view = _cameraTransform3d.lock()->GetModel();

    for (const auto& value : _sprites)
    {

        if (const auto& component = value.lock())
        {
            component->Update(deltaTime); // move to material

            const auto model = component->GetModel();
            component->SetUniformMat4("model",model);
            component->SetUniformMat4("view",view);
            component->SetUniformMat4("projection",projection);

            component->Render();
        }
    }
}

void RenderPipeline::RenderMeshes(const float& deltaTime) {

    glDisable(GL_CULL_FACE); // spine runtime requires

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

            meshRenderer->Update(deltaTime);

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

#ifndef NDEBUG
void RenderPipeline::RenderDebugLines() const {

    const auto projection = _camera3d.lock()->GetProjection();
    const auto view = _cameraTransform3d.lock()->GetModel();

    DebugLines::UpdateViewProjection(view, projection);
    DebugLines::DrawLines();
    DebugLines::Clear();
}

#endif


void RenderPipeline::Init() const noexcept {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

#ifndef NDEBUG
    DebugLines::Init();
#endif
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
#ifndef NDEBUG
    DebugLines::Clear();
#endif
}
