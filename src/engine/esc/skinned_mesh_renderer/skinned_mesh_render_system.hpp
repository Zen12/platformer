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

            for (const auto &[entity, skinnedMesh, transform] : View.each()) {
                // Initialize bone transforms to bind pose if not yet initialized
                // (for meshes without animation components)
                static bool initialized = false;
                if (!initialized && skinnedMesh.BoneOffsets.size() > 0) {
                    // Check if this mesh has identity transforms (not yet initialized by animation)
                    bool needsInit = true;
                    if (skinnedMesh.BoneTransforms.size() > 0) {
                        // Check if first bone is identity - if not, assume it's been initialized
                        const auto& firstBone = skinnedMesh.BoneTransforms[0];
                        if (firstBone != glm::mat4(1.0f)) {
                            needsInit = false;
                        }
                    }

                    if (needsInit) {
                        // Set bind pose: just use bone offsets directly
                        // The mesh is in bind pose when bone transforms equal bone offsets
                        for (size_t i = 0; i < skinnedMesh.BoneOffsets.size() && i < skinnedMesh.BoneTransforms.size(); i++) {
                            skinnedMesh.BoneTransforms[i] = skinnedMesh.BoneOffsets[i];
                        }
                    }
                }

                const auto &model = transform.GetModel();

                // Extract position from model matrix (last column)
                const glm::vec3 position(model[3][0], model[3][1], model[3][2]);

                // Use a conservative bounding sphere radius
                // TODO: Get actual mesh bounds from mesh data
                //constexpr float boundingRadius = 5.0f;

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
