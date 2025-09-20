#include "ui_image_renderer.hpp"

void UiImageRenderer::Update([[maybe_unused]] const float& deltaTime)
{
}

void UiImageRenderer::SetFillAmount(const float &amount) noexcept {
    _fillAmount = amount;
}

void UiImageRenderer::SetTint(const glm::vec4& color) {
    _tint = color;
}

void UiImageRenderer::SetSprite(const std::weak_ptr<Sprite> &sprite) noexcept {
    _sprite = sprite;
}

void UiImageRenderer::Render(const glm::mat4 &projection) const
{
    if (const auto entity = _entity.lock()) {

        if (const auto transform = entity->GetComponent<RectTransform>().lock()) {

            if (const auto material = _material.lock())
            {
                const auto model = transform->GetModel();

                material->ClearSprites();
                material->AddSprite(_sprite);

                material->SetMat4("projection", projection);
                material->SetMat4("model", model);
                material->SetFloat("fillAmount", _fillAmount);
                material->SetVec4("tint", _tint);

                material->Bind();
                _mesh.Bind();

                glDrawElements(GL_TRIANGLES, static_cast<int32_t>(_mesh.GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
            }

        }
    }

}
