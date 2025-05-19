#include "ui_image_renderer.hpp"

void UiImageRenderer::Update() const
{
    if (auto material = _material.lock())
    {
        material->Bind();
        _mesh.Bind();
    }
}

void UiImageRenderer::Render(const glm::mat4 &projection) const
{
    if (auto material = _material.lock())
    {
        auto model = GetEntity().lock()->GetComponent<RectTransform>().lock()->GetModel();

        material->SetMat4("projection", projection);
        material->SetMat4("model", model);
    }

    glDrawElements(GL_TRIANGLES, (int32_t)_mesh.GetIndicesCount(), GL_UNSIGNED_INT, 0);
}
