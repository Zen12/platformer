#pragma once

#include <entt/entity/registry.hpp>
#include "sprite_component.hpp"
#include "../camera/camera_component.hpp"
#include "../transform/transform_component.hpp"
#include "../esc_core.hpp"
#include "../../scene/scene.hpp"


class SpriteRenderSystem final : public ISystem {
private:
    const decltype(std::declval<entt::registry>().view<SpriteComponentV2, TransformComponentV2>()) _spriteView;
    const decltype(std::declval<entt::registry>().view<CameraComponentV2>()) _cameraView;
    const std::weak_ptr<Scene> _scene;

public:
    SpriteRenderSystem(
        const decltype(std::declval<entt::registry>().view<SpriteComponentV2, TransformComponentV2>()) &spriteView,
        const decltype(std::declval<entt::registry>().view<CameraComponentV2>()) &cameraView,
        const std::weak_ptr<Scene> &scene
    ) : ISystem(), _spriteView(spriteView), _cameraView(cameraView), _scene(scene) {
    }


    void OnTick() override {
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        for (const auto &[_, camera] : _cameraView.each()) {
            const auto projection = camera.Projection;
            const auto view = camera.View;

            for (auto [entity, sprite, transform] : _spriteView.each()) {
                const auto &model = transform.GetModel();

                const auto spriteData = _scene.lock()->GetSprite(sprite.Data.SpriteGuid);
                const auto mat = _scene.lock()->GetMaterial(sprite.Data.MaterialGuid);
                const auto mesh = _scene.lock()->GetMesh("TEMP");

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
};