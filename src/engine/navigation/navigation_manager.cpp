#include "navigation_manager.hpp"

void NavigationManager::Initialize(int width, int height, float cellSize, const glm::vec3& origin,
                                  const std::vector<std::vector<int>>& walkabilityGrid, int maxAgents) {
    _navmesh = std::make_shared<DetourNavmesh>();

    // Initialize navmesh with walkability grid
    if (!_navmesh->Initialize(width, height, cellSize, origin, walkabilityGrid)) {
        _navmesh.reset();
        return;
    }

    // Initialize crowd
    _crowd = std::make_shared<DetourCrowd>();
    if (!_crowd->Initialize(_navmesh, maxAgents)) {
        _crowd.reset();
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
