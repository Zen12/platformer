#pragma once
#include "GL/glew.h"
#include "mesh_renderer_component.hpp"
#include "../esc_core.hpp"
#include "../../renderer/render_repository.hpp"
#include "../../scene/scene.hpp"
#include "../camera/camera_component.hpp"
#include "../transform/transform_component.hpp"


class MeshRenderSystem final : public ISystemView<MeshRendererComponent, TransformComponentV2>{
private:

    using TypeCamera = decltype(std::declval<entt::registry>().view<CameraComponentV2>());


    const TypeCamera _cameraView;
    const std::shared_ptr<RenderRepository> _repository;

public:
    explicit MeshRenderSystem(
        const TypeView &view,
        const TypeCamera &camera,
        const std::shared_ptr<RenderRepository> &repository)
        : ISystemView(view) , _cameraView(camera), _repository(repository) {
    }

    void OnTick() override {
        for (const auto &[_, camera] : _cameraView.each()) {
            for (const auto &[_, mesh, transform] : View.each()) {
                const auto &model = transform.GetModel();
                _repository->Add(RenderData{mesh.MaterialGuid, mesh.Guid, model, camera.View, camera.Projection});
            }
        }
    }
};
