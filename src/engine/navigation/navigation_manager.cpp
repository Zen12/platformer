#include "navigation_manager.hpp"

void NavigationManager::Initialize(int width, int height, float cellSize, const glm::vec3& origin,
                                  const std::vector<std::vector<int>>& walkabilityGrid, int maxAgents) {
    _navmesh = std::make_shared<GridNavmesh>();

    // Initialize navmesh with walkability grid
    if (!_navmesh->Initialize(width, height, cellSize, origin, walkabilityGrid)) {
        _navmesh.reset();
        return;
    }

    // Initialize RVO2 crowd for collision avoidance
    _crowd = std::make_shared<RVO2Crowd>();
    if (!_crowd->Initialize(maxAgents)) {
        _crowd.reset();
    } else {
        // Initialize spatial grid with same dimensions as navmesh for efficient raycasting
        _crowd->InitializeSpatialGrid(width, height, cellSize, origin);
    }
}

void NavigationManager::Update(float deltaTime) {
    if (_crowd) {
        _crowd->Update(deltaTime);
    }
}

void NavigationManager::Clear() {
    if (_crowd) {
        _crowd->Clear();
    }
    _navmesh.reset();
    _crowd.reset();
}
