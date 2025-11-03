#pragma once

class RenderSystem final {

    CameraComponentComponent _camera{};
    TransformComponent _cameraTransform{};

public:
    RenderSystem(const CameraComponentComponent &camera, const TransformComponent &cameraTransform) :
        _camera(camera), _cameraTransform(cameraTransform)
    {}

    void RenderSprites(
        const decltype(std::declval<entt::registry>().view<TransformComponent, SpriteComponent>())& spriteView) {
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        const auto projection = _camera.GetProjection();
        const auto view = _cameraTransform.GetModel();

        for (auto [entity, transform, sprite] : spriteView.each()) {
            const auto &model = transform.GetModel();
            const auto spriteData = sprite.GetSprite();
            const auto materialData = sprite.GetMaterial();
            if (const auto& mat = materialData.lock()) {

                if (const auto& mesh = sprite.GetMesh().lock()) {
                    mat->ClearSprites();
                    mat->AddSprite(spriteData);
                    mat->SetMat4("model", model);
                    mat->SetMat4("view", view);
                    mat->SetMat4("projection", projection);
                    mat->Bind();
                    mesh->Bind();
                    glDrawElements(GL_TRIANGLES, static_cast<int32_t>(mesh->GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
                }
            }
        }
    }
};