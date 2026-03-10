#pragma once
#include "GL/glew.h"
#include "mesh_renderer_component.hpp"
#include "esc/esc_core.hpp"
#include "buffer/render_buffer.hpp"
#include "buffer/render_buffer_component.hpp"
#include "frustum.hpp"
#include "resource_cache.hpp"
#include "camera/camera_component.hpp"
#include "transform/transform_component.hpp"


class MeshRenderSystem final : public ISystemView<MeshRendererComponent, TransformComponentV2>{
private:

    using TypeCamera = decltype(std::declval<entt::registry>().view<CameraComponentV2>());
    using TypeRenderBuffer = decltype(std::declval<entt::registry>().view<RenderBufferComponent>());


    const TypeCamera _cameraView;
    const TypeRenderBuffer _renderBufferView;
    const std::shared_ptr<ResourceCache> _resourceCache;

public:
    explicit MeshRenderSystem(
        const TypeView &view,
        const TypeCamera &camera,
        const TypeRenderBuffer &renderBufferView,
        const std::shared_ptr<ResourceCache> &resourceCache)
        : ISystemView(view) , _cameraView(camera), _renderBufferView(renderBufferView), _resourceCache(resourceCache) {
    }

    void OnTick() override {
        std::shared_ptr<RenderBuffer> _repository;
        for (const auto& [_, rb] : _renderBufferView.each()) { _repository = rb.Buffer; }
        if (!_repository) return;

        for (const auto &[_, camera] : _cameraView.each()) {
            // Extract frustum from view-projection matrix
            Frustum frustum;
            const glm::mat4 viewProjection = camera.Projection * camera.View;
            frustum.ExtractPlanes(viewProjection);

            for (const auto &[_, mesh, transform] : View.each()) {
                const auto &model = transform.GetModel();

                if (!mesh.MeshBounds.IsValid()) {
                    _resourceCache->GetMesh(mesh.MeshGuid);
                    mesh.MeshBounds = _resourceCache->GetMeshBounds(mesh.MeshGuid);
                }

                if (mesh.MeshBounds.IsValid()) {
                    const Bounds worldBounds = mesh.MeshBounds.Transform(model);
                    if (!frustum.IsBoxVisible(worldBounds.Center, worldBounds.Extents)) {
                        continue;
                    }
                }

                _repository->Add(RenderData{
                    mesh.MaterialGuid,
                    mesh.MeshGuid,
                    model,
                    camera.View,
                    camera.Projection,
                    std::nullopt,  // BoneTransforms
                    PrimitiveType::Triangles,
                    std::nullopt,  // Positions
                    mesh.Color,    // LineColor (used for solid color materials)
                    std::nullopt,  // InstanceColor
                    false,  // IsSkinned
                    camera.cameraData.yDepthFactor  // Y-based depth offset
                });
            }
        }
    }
};
