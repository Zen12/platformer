#pragma once
#include "navmesh_grid.hpp"
#include "rvo_crowd.hpp"
#include <memory>

class NavigationManager {
private:
    std::shared_ptr<NavmeshGrid> _navmesh;
    std::shared_ptr<RVOCrowd> _crowd;

public:
    NavigationManager() = default;

    void Initialize(int width, int height, float cellSize, const glm::vec3& origin,
                    const std::vector<std::vector<int>>& walkabilityGrid = {});

    void Update(float deltaTime);

    [[nodiscard]] std::shared_ptr<NavmeshGrid> GetNavmesh() const noexcept { return _navmesh; }
    [[nodiscard]] std::shared_ptr<RVOCrowd> GetCrowd() const noexcept { return _crowd; }

    void BakeNavmeshFromGeometry(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices);

    void Clear();
};
