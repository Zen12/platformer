#include "ui_image_renderer.h"

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
        //model = glm::translate(model, glm::vec3(0.0f, 0, -1));
        //model = glm::scale(model, glm::vec3(200.0f));

        material->SetMat4("projection", projection);
        material->SetMat4("model", model);
    }

    glDrawElements(GL_TRIANGLES, _mesh.GetIndicesCount(), GL_UNSIGNED_INT, 0);
}
