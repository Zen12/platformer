#include "navigation_manager.hpp"

void NavigationManager::Initialize(int width, int height, float cellSize, const glm::vec3& origin,
                                  const std::vector<std::vector<int>>& walkabilityGrid) {
    _navmesh = std::make_shared<NavmeshGrid>(width, height, cellSize, origin);
    _crowd = std::make_shared<RVOCrowd>();

    // Apply walkability grid if provided
    if (!walkabilityGrid.empty()) {
        const int gridHeight = static_cast<int>(walkabilityGrid.size());
        for (int z = 0; z < gridHeight && z < height; ++z) {
            const auto& row = walkabilityGrid[z];
            const int gridWidth = static_cast<int>(row.size());
            for (int x = 0; x < gridWidth && x < width; ++x) {
                const bool walkable = (row[x] != 0); // 0 = unwalkable, non-zero = walkable
                _navmesh->SetCellWalkable(x, z, walkable);
            }
        }
    }
}

void NavigationManager::Update(float deltaTime) {
    if (_crowd) {
        _crowd->UpdateAgents(deltaTime);
    }
}

void NavigationManager::BakeNavmeshFromGeometry(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices) {
    if (_navmesh) {
        _navmesh->BakeFromGeometry(vertices, indices);
    }
}

void NavigationManager::Clear() {
    if (_crowd) {
        _crowd->Clear();
    }
    _navmesh.reset();
    _crowd.reset();
}
