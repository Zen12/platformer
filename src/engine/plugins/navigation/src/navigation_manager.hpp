#pragma once
#include "grid_navmesh.hpp"
#include "rvo2_crowd.hpp"
#include <memory>

class NavigationManager {
private:
    std::shared_ptr<GridNavmesh> _navmesh;
    std::shared_ptr<RVO2Crowd> _crowd;

public:
    NavigationManager() = default;

    void Initialize(int width, int height, float cellSize, const glm::vec3& origin,
                    const std::vector<std::vector<int>>& walkabilityGrid = {},
                    int maxAgents = 2000);

    void Update(float deltaTime);

    [[nodiscard]] std::shared_ptr<GridNavmesh> GetNavmesh() const noexcept { return _navmesh; }
    [[nodiscard]] std::shared_ptr<RVO2Crowd> GetCrowd() const noexcept { return _crowd; }

    void Clear();
};
