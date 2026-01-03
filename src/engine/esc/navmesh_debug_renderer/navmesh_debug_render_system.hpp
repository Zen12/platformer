#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "../esc_core.hpp"
#include "../camera/camera_component.hpp"
#include "../../renderer/render_repository.hpp"
#include "../../navigation/navigation_manager.hpp"
#include <DetourNavMesh.h>

class NavmeshDebugRenderSystem final : public ISystem {
private:
    using TypeCamera = decltype(std::declval<entt::registry>().view<CameraComponentV2>());

    const TypeCamera _cameraView;
    std::shared_ptr<RenderRepository> _repository;
    std::shared_ptr<NavigationManager> _navigationManager;
    std::string _materialGuid = "b53dc63c-0c57-46b1-80e5-7570f2445f8a"; // line.mat

    [[nodiscard]] std::vector<glm::vec3> ExtractNavmeshPolygons() const noexcept {
        if (!_navigationManager) {
            return {};
        }

        const auto navmesh = _navigationManager->GetNavmesh();
        if (!navmesh || !navmesh->GetNavMesh()) {
            return {};
        }

        std::vector<glm::vec3> positions;
        const dtNavMesh* dtNavMeshPtr = navmesh->GetNavMesh();

        // Iterate through all tiles
        for (int i = 0; i < dtNavMeshPtr->getMaxTiles(); ++i) {
            const dtMeshTile* tile = dtNavMeshPtr->getTile(i);
            if (!tile || !tile->header) continue;

            // Iterate through all polygons in the tile
            for (int j = 0; j < tile->header->polyCount; ++j) {
                const dtPoly* poly = &tile->polys[j];

                // Only draw walkable polygons
                if (poly->getType() != DT_POLYTYPE_GROUND) continue;

                // Draw polygon edges
                for (int k = 0; k < poly->vertCount; ++k) {
                    const float* v0 = &tile->verts[poly->verts[k] * 3];
                    const float* v1 = &tile->verts[poly->verts[(k + 1) % poly->vertCount] * 3];

                    // Add line from v0 to v1
                    positions.emplace_back(v0[0], v0[1] + 0.1f, v0[2]);
                    positions.emplace_back(v1[0], v1[1] + 0.1f, v1[2]);
                }
            }
        }

        return positions;
    }

public:
    NavmeshDebugRenderSystem(
        const TypeCamera &cameraView,
        const std::shared_ptr<RenderRepository> &repository,
        const std::shared_ptr<NavigationManager> &navigationManager)
        : _cameraView(cameraView),
          _repository(repository), _navigationManager(navigationManager) {}

    void OnTick() override {
        const auto positions = ExtractNavmeshPolygons();

        if (positions.empty()) {
            return;
        }

        for (const auto &[_, camera] : _cameraView.each()) {
            _repository->Add(RenderData{
                _materialGuid,
                "",  // No mesh GUID needed for direct position rendering
                glm::mat4(1.0f),
                camera.View,
                camera.Projection,
                std::nullopt,
                PrimitiveType::Lines,
                positions,
                glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)  // Blue color for navmesh
            });
        }
    }
};
