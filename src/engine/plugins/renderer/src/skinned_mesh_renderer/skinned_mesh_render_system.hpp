#pragma once
#include "GL/glew.h"
#include "skinned_mesh_renderer_component.hpp"
#include "esc/esc_core.hpp"
#include "buffer/render_buffer.hpp"
#include "buffer/render_buffer_component.hpp"
#include "frustum.hpp"
#include "resource_cache.hpp"
#include "camera/camera_component.hpp"
#include "transform/transform_component.hpp"
#include <algorithm>

#define DEBUG_DRAW_BOUNDS 0

class SkinnedMeshRenderSystem final : public ISystemView<SkinnedMeshRendererComponent, TransformComponentV2>{
private:
    // Line material GUID for debug rendering
    static constexpr const char* LINE_MATERIAL_GUID = "b53dc63c-0c57-46b1-80e5-7570f2445f8a";

    using TypeCamera = decltype(std::declval<entt::registry>().view<CameraComponentV2>());
    using TypeRenderBuffer = decltype(std::declval<entt::registry>().view<RenderBufferComponent>());

    // Generate line vertices for a bounding box (12 edges = 24 vertices)
    static std::vector<glm::vec3> GenerateBoxLines(const glm::vec3& center, const glm::vec3& extents) {
        std::vector<glm::vec3> lines;
        lines.reserve(24);

        // 8 corners of the box
        const glm::vec3 min = center - extents;
        const glm::vec3 max = center + extents;

        // Bottom face edges (4 edges)
        lines.push_back({min.x, min.y, min.z}); lines.push_back({max.x, min.y, min.z});
        lines.push_back({max.x, min.y, min.z}); lines.push_back({max.x, min.y, max.z});
        lines.push_back({max.x, min.y, max.z}); lines.push_back({min.x, min.y, max.z});
        lines.push_back({min.x, min.y, max.z}); lines.push_back({min.x, min.y, min.z});

        // Top face edges (4 edges)
        lines.push_back({min.x, max.y, min.z}); lines.push_back({max.x, max.y, min.z});
        lines.push_back({max.x, max.y, min.z}); lines.push_back({max.x, max.y, max.z});
        lines.push_back({max.x, max.y, max.z}); lines.push_back({min.x, max.y, max.z});
        lines.push_back({min.x, max.y, max.z}); lines.push_back({min.x, max.y, min.z});

        // Vertical edges (4 edges)
        lines.push_back({min.x, min.y, min.z}); lines.push_back({min.x, max.y, min.z});
        lines.push_back({max.x, min.y, min.z}); lines.push_back({max.x, max.y, min.z});
        lines.push_back({max.x, min.y, max.z}); lines.push_back({max.x, max.y, max.z});
        lines.push_back({min.x, min.y, max.z}); lines.push_back({min.x, max.y, max.z});

        return lines;
    }


    const TypeCamera _cameraView;
    const TypeRenderBuffer _renderBufferView;
    const std::shared_ptr<ResourceCache> _resourceCache;

public:
    explicit SkinnedMeshRenderSystem(
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

                // Lazy-load bounds from ResourceCache on first render
                if (!skinnedMesh.MeshBounds.IsValid()) {
                    skinnedMesh.MeshBounds = _resourceCache->GetValue<Bounds>(skinnedMesh.MeshGuid);
                }

                // Transform bounds to world space and perform AABB frustum culling
                // For skinned meshes, expand bounds to account for animation deformation
                Bounds worldBounds;
                if (skinnedMesh.MeshBounds.IsValid()) {
                    Bounds expandedBounds = skinnedMesh.MeshBounds;
                    // Expand bounds and ensure minimum extent for animations
                    // Animations can move vertices significantly (arms, legs, leaning, etc.)
                    constexpr float minExtent = 0.5f;  // Minimum half-size in each dimension
                    expandedBounds.Extents.x = std::max(expandedBounds.Extents.x * 1.5f, minExtent);
                    expandedBounds.Extents.y = std::max(expandedBounds.Extents.y * 1.5f, minExtent);
                    expandedBounds.Extents.z = std::max(expandedBounds.Extents.z * 1.5f, minExtent);

                    worldBounds = expandedBounds.Transform(model);
                    if (!frustum.IsBoxVisible(worldBounds.Center, worldBounds.Extents)) {
                        continue; // Skip this mesh, it's outside the frustum
                    }
                }

                // Pass bone transforms to renderer with IsSkinned = true for instanced rendering
                _repository->Add(RenderData{
                    skinnedMesh.MaterialGuid,
                    skinnedMesh.MeshGuid,
                    model,
                    camera.View,
                    camera.Projection,
                    skinnedMesh.BoneTransforms,
                    PrimitiveType::Triangles,
                    std::nullopt,  // Positions
                    std::nullopt,  // LineColor
                    std::nullopt,  // InstanceColor
                    true,  // IsSkinned = true
                    camera.cameraData.yDepthFactor  // Y-based depth offset
                });

#if DEBUG_DRAW_BOUNDS
                // Render debug bounds as green wireframe box
                if (worldBounds.IsValid()) {
                    auto boxLines = GenerateBoxLines(worldBounds.Center, worldBounds.Extents);
                    _repository->Add(RenderData{
                        LINE_MATERIAL_GUID,
                        "",  // No mesh needed for lines
                        glm::mat4(1.0f),  // Identity - positions are already in world space
                        camera.View,
                        camera.Projection,
                        std::nullopt,  // BoneTransforms
                        PrimitiveType::Lines,
                        boxLines,
                        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),  // LineColor - Green
                        std::nullopt,  // InstanceColor
                        false  // IsSkinned
                    });
                }
#endif
            }
        }
    }
};
