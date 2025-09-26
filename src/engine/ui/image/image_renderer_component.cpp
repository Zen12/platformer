#include "image_renderer_component.hpp"

UiImageRendererComponent::UiImageRendererComponent(const std::weak_ptr<Entity> &entity)
    : Component(entity), _mesh(Mesh::GenerateUI())
{
}

void UiImageRendererComponent::Update([[maybe_unused]] const float& deltaTime)
{
}

void UiImageRendererComponent::Render(const glm::mat4 &projection) const
{
    if (const auto &material = _material.lock())
    {
        const auto model = GetEntity().lock()->GetComponent<RectTransform>().lock()->GetModel();

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
