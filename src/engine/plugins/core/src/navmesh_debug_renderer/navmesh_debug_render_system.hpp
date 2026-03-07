#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "esc/esc_core.hpp"
#include "../camera/camera_component.hpp"
#include "render_repository.hpp"
#include "navigation_manager.hpp"
#include "guid.hpp"

class NavmeshDebugRenderSystem final : public ISystem {
private:
    using TypeCamera = decltype(std::declval<entt::registry>().view<CameraComponentV2>());

    const TypeCamera _cameraView;
    std::shared_ptr<RenderRepository> _repository;
    std::shared_ptr<NavigationManager> _navigationManager;
    Guid _materialGuid = Guid::FromString("b53dc63c-0c57-46b1-80e5-7570f2445f8a");

    [[nodiscard]] std::vector<glm::vec3> ExtractNavmeshPolygons() const noexcept {
        if (!_navigationManager) {
            return {};
        }

        const auto navmesh = _navigationManager->GetNavmesh();
        if (!navmesh) {
            return {};
        }

        const auto& grid = navmesh->GetGrid();
        if (grid.empty()) {
            return {};
        }

        const glm::vec3 origin = navmesh->GetOrigin();
        const float cellSize = navmesh->GetCellSize();
        const int width = navmesh->GetWidth();
        const int height = navmesh->GetHeight();
        const float yOffset = 0.1f;

        std::vector<glm::vec3> positions;

        for (int z = 0; z < height && z < static_cast<int>(grid.size()); ++z) {
            for (int x = 0; x < width && x < static_cast<int>(grid[z].size()); ++x) {
                if (grid[z][x] == 0) continue;

                const float x0 = origin.x + static_cast<float>(x) * cellSize;
                const float x1 = origin.x + static_cast<float>(x + 1) * cellSize;
                const float z0 = origin.z + static_cast<float>(z) * cellSize;
                const float z1 = origin.z + static_cast<float>(z + 1) * cellSize;
                const float y = origin.y + yOffset;

                if (x == 0 || grid[z][x - 1] == 0) {
                    positions.emplace_back(x0, y, z0);
                    positions.emplace_back(x0, y, z1);
                }
                if (x == width - 1 || x + 1 >= static_cast<int>(grid[z].size()) || grid[z][x + 1] == 0) {
                    positions.emplace_back(x1, y, z0);
                    positions.emplace_back(x1, y, z1);
                }
                if (z == 0 || grid[z - 1][x] == 0) {
                    positions.emplace_back(x0, y, z0);
                    positions.emplace_back(x1, y, z0);
                }
                if (z == height - 1 || z + 1 >= static_cast<int>(grid.size()) || grid[z + 1][x] == 0) {
                    positions.emplace_back(x0, y, z1);
                    positions.emplace_back(x1, y, z1);
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
                Guid{},
                glm::mat4(1.0f),
                camera.View,
                camera.Projection,
                std::nullopt,
                PrimitiveType::Lines,
                positions,
                glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                std::nullopt,
                false
            });
        }
    }
};
