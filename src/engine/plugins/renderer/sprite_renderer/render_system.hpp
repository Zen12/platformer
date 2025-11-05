#pragma once
#include <utility>

#include "../../../renderer/sprite/sprite_renderer_component_serialization.hpp"
#include "../common/camera_plugin.hpp"
#include "../../../scene/scene.hpp"
#include "GL/glew.h"

namespace Plugins::Renderer::Sprite {


    class SpriteComponentV2 final {
        public:
            const SpriteRenderComponentSerialization Data;

        explicit SpriteComponentV2(SpriteRenderComponentSerialization data)
            : Data(std::move(data))
        {}
    };


    class SpriteRenderSystem final : public Core::ISystem {
    private:
        const decltype(std::declval<entt::registry>().view<SpriteComponentV2, Core::TransformComponentV2>()) _spriteView;
        const decltype(std::declval<entt::registry>().view<Common::CameraComponentV2>()) _cameraView;
        const std::weak_ptr<Scene> _scene;

    public:
        SpriteRenderSystem(
            const decltype(std::declval<entt::registry>().view<SpriteComponentV2, Core::TransformComponentV2>()) &spriteView,
            const decltype(std::declval<entt::registry>().view<Common::CameraComponentV2>()) &cameraView,
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
}