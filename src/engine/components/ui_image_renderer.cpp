#include "ui_image_renderer.hpp"

void UiImageRenderer::Update() const
{
    if (const auto material = _material.lock())
    {
        material->Bind();
        _mesh.Bind();
    }
}

void UiImageRenderer::SetSprite(const std::weak_ptr<Sprite> &sprite) const {
    if (const auto material = _material.lock())
    {
        material->AddSprite(sprite);
    }
}

void UiImageRenderer::Render(const glm::mat4 &projection) const
{
    if (const auto material = _material.lock())
    {
        // ???
        const auto model = GetEntity().lock()->GetComponent<RectTransform>().lock()->GetModel();

        material->SetMat4("projection", projection);
        material->SetMat4("model", model);
    }

    glDrawElements(GL_TRIANGLES, static_cast<int32_t>(_mesh.GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
}
