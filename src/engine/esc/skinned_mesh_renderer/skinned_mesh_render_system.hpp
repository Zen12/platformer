#pragma once
#include "GL/glew.h"
#include "skinned_mesh_renderer_component.hpp"
#include "../esc_core.hpp"
#include "../../renderer/render_repository.hpp"
#include "../../renderer/frustum.hpp"
#include "../../scene/scene.hpp"
#include "../camera/camera_component.hpp"
#include "../transform/transform_component.hpp"


class SkinnedMeshRenderSystem final : public ISystemView<SkinnedMeshRendererComponent, TransformComponentV2>{
private:

    using TypeCamera = decltype(std::declval<entt::registry>().view<CameraComponentV2>());


    const TypeCamera _cameraView;
    const std::shared_ptr<RenderRepository> _repository;

public:
    explicit SkinnedMeshRenderSystem(
        const TypeView &view,
        const TypeCamera &camera,
        const std::shared_ptr<RenderRepository> &repository)
        : ISystemView(view) , _cameraView(camera), _repository(repository) {
    }

    void OnTick() override {
        for (const auto &[_, camera] : _cameraView.each()) {
            // Extract frustum from view-projection matrix
            Frustum frustum;
            const glm::mat4 viewProjection = camera.Projection * camera.View;
            frustum.ExtractPlanes(viewProjection);

            for (const auto &[_, skinnedMesh, transform] : View.each()) {
                const auto &model = transform.GetModel();

                // Extract position from model matrix (last column)
                const glm::vec3 position(model[3][0], model[3][1], model[3][2]);

                // Use a conservative bounding sphere radius
                // TODO: Get actual mesh bounds from mesh data
                constexpr float boundingRadius = 5.0f;

                // Perform frustum culling
                //if (!frustum.IsSphereVisible(position, boundingRadius)) {
                //    continue; // Skip this mesh, it's outside the frustum
                //}

                // Pass bone transforms to renderer
                _repository->Add(RenderData{skinnedMesh.MaterialGuid, skinnedMesh.Guid, model, camera.View, camera.Projection, skinnedMesh.BoneTransforms});
            }
        }
    }
};
