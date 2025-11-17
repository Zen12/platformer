#pragma once
#include "GL/glew.h"
#include "mesh_renderer_component.hpp"
#include "../esc_core.hpp"
#include "../../scene/scene.hpp"
#include "../camera/camera_component.hpp"
#include "../transform/transform_component.hpp"


class MeshRenderSystem final : public ISystemView<MeshRendererComponent, TransformComponentV2>{
private:

    using TypeCamera = decltype(std::declval<entt::registry>().view<CameraComponentV2>());


    const TypeCamera _cameraView;
    const std::weak_ptr<Scene> _scene;

public:
    explicit MeshRenderSystem(
        const TypeView &view,
        const TypeCamera &camera,
        const std::weak_ptr<Scene> &scene)
        : ISystemView(view) , _cameraView(camera), _scene(scene) {
    }

    void OnTick() override {
        for (const auto &[_, camera] : _cameraView.each()) {
            const auto projection = camera.Projection;
            const auto view = camera.View;

            for (const auto &[_, mesh, transform] : View.each()) {
                const auto &model = transform.GetModel();

                const auto mat = _scene.lock()->GetMaterial(mesh.MaterialGuid);
                const auto meshPtr = _scene.lock()->GetMesh(mesh.Guid);

                //mat->ClearSprites();
                mat->SetMat4("model", model);
                mat->SetMat4("view", view);
                mat->SetMat4("projection", projection);
                mat->Bind();
                meshPtr->Bind();
                glDrawElements(GL_TRIANGLES, static_cast<int32_t>(meshPtr->GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
            }
        }
    }
};
