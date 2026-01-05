#pragma once
#include "GL/glew.h"
#include "mesh_renderer_component.hpp"
#include "../esc_core.hpp"
#include "../../renderer/render_repository.hpp"
#include "../../renderer/frustum.hpp"
#include "../../scene/scene.hpp"
#include "../camera/camera_component.hpp"
#include "../transform/transform_component.hpp"


class MeshRenderSystem final : public ISystemView<MeshRendererComponent, TransformComponentV2>{
private:

    using TypeCamera = decltype(std::declval<entt::registry>().view<CameraComponentV2>());


    const TypeCamera _cameraView;
    const std::shared_ptr<RenderRepository> _repository;
    const std::weak_ptr<Scene> _scene;

public:
    explicit MeshRenderSystem(
        const TypeView &view,
        const TypeCamera &camera,
        const std::shared_ptr<RenderRepository> &repository,
        const std::weak_ptr<Scene> &scene)
        : ISystemView(view) , _cameraView(camera), _repository(repository), _scene(scene) {
    }

    void OnTick() override {
        auto scene = _scene.lock();

        for (const auto &[_, camera] : _cameraView.each()) {
            // Extract frustum from view-projection matrix
            Frustum frustum;
            const glm::mat4 viewProjection = camera.Projection * camera.View;
            frustum.ExtractPlanes(viewProjection);

            for (const auto &[_, mesh, transform] : View.each()) {
                const auto &model = transform.GetModel();

                // Lazy-load bounds from Scene on first render
                if (!mesh.MeshBounds.IsValid() && scene) {
                    scene->GetMesh(mesh.Guid);  // Ensure mesh is loaded
                    mesh.MeshBounds = scene->GetMeshBounds(mesh.Guid);
                }

                // Transform bounds to world space and perform AABB frustum culling
                if (mesh.MeshBounds.IsValid()) {
                    const Bounds worldBounds = mesh.MeshBounds.Transform(model);
                    if (!frustum.IsBoxVisible(worldBounds.Center, worldBounds.Extents)) {
                        continue; // Skip this mesh, it's outside the frustum
                    }
                }

                _repository->Add(RenderData{mesh.MaterialGuid, mesh.Guid, model, camera.View, camera.Projection, std::nullopt});
            }
        }
    }
};
