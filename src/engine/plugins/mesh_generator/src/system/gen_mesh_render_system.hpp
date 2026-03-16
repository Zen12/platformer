#pragma once
#include "component/gen_mesh_renderer_component.hpp"
#include "esc/esc_core.hpp"
#include "buffer/render_buffer.hpp"
#include "buffer/render_buffer_component.hpp"
#include "frustum.hpp"
#include "resource_factory.hpp"
#include "mesh/mesh.hpp"
#include "camera/camera_component.hpp"
#include "transform/transform_component.hpp"

class GenMeshRenderSystem final : public ISystemView<GenMeshRendererComponent, TransformComponentV2> {
private:
    using TypeCamera = decltype(std::declval<entt::registry>().view<CameraComponentV2>());
    using TypeRenderBuffer = decltype(std::declval<entt::registry>().view<RenderBufferComponent>());

    const TypeCamera _cameraView;
    const TypeRenderBuffer _renderBufferView;
    const std::shared_ptr<ResourceFactory> _resourceFactory;

public:
    explicit GenMeshRenderSystem(
        const TypeView& view,
        const TypeCamera& camera,
        const TypeRenderBuffer& renderBufferView,
        const std::shared_ptr<ResourceFactory>& resourceFactory)
        : ISystemView(view), _cameraView(camera), _renderBufferView(renderBufferView), _resourceFactory(resourceFactory) {
    }

    void OnTick() override {
        std::shared_ptr<RenderBuffer> repository;
        for (const auto& [_, rb] : _renderBufferView.each()) { repository = rb.Buffer; }
        if (!repository) return;

        for (const auto& [_, camera] : _cameraView.each()) {
            Frustum frustum;
            const glm::mat4 viewProjection = camera.Projection * camera.View;
            frustum.ExtractPlanes(viewProjection);

            for (const auto& [_, mesh, transform] : View.each()) {
                if (!_resourceFactory->Get<Mesh>(mesh.MeshGuid)) continue;
                const auto& model = transform.GetModel();

                if (!mesh.MeshBounds.IsValid()) {
                    mesh.MeshBounds = _resourceFactory->GetValue<Bounds>(mesh.MeshGuid);
                }

                if (mesh.MeshBounds.IsValid()) {
                    const Bounds worldBounds = mesh.MeshBounds.Transform(model);
                    if (!frustum.IsBoxVisible(worldBounds.Center, worldBounds.Extents)) {
                        continue;
                    }
                }

                repository->Add(RenderData{
                    mesh.MaterialGuid,
                    mesh.MeshGuid,
                    model,
                    camera.View,
                    camera.Projection,
                    std::nullopt,
                    PrimitiveType::Triangles,
                    std::nullopt,
                    mesh.Color,
                    std::nullopt,
                    false,
                    camera.cameraData.yDepthFactor
                });
            }
        }
    }
};
