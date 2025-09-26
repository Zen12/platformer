#include "image_renderer_component.hpp"

void UiImageRendererComponent::Update([[maybe_unused]] const float& deltaTime)
{
}

void UiImageRendererComponent::SetFillAmount(const float &amount) noexcept {
    _fillAmount = amount;
}

void UiImageRendererComponent::SetTint(const glm::vec4& color) {
    _tint = color;
}

void UiImageRendererComponent::SetSprite(const std::weak_ptr<Sprite> &sprite) noexcept {
    _sprite = sprite;
}

void UiImageRendererComponent::Render(const glm::mat4 &projection) const
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
