#pragma once
#include "detour_navmesh.hpp"
#include "detour_crowd.hpp"
#include <memory>

class NavigationManager {
private:
    std::shared_ptr<DetourNavmesh> _navmesh;
    std::shared_ptr<DetourCrowd> _crowd;

public:
    NavigationManager() = default;

    void Initialize(int width, int height, float cellSize, const glm::vec3& origin,
                    const std::vector<std::vector<int>>& walkabilityGrid = {},
                    int maxAgents = 2000);

    void Update(float deltaTime);

    [[nodiscard]] std::shared_ptr<DetourNavmesh> GetNavmesh() const noexcept { return _navmesh; }
    [[nodiscard]] std::shared_ptr<DetourCrowd> GetCrowd() const noexcept { return _crowd; }

    void Clear();
};
