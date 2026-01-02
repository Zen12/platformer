#include "navigation_manager.hpp"

void NavigationManager::Initialize(int width, int height, float cellSize, const glm::vec3& origin) {
    _navmesh = std::make_shared<NavmeshGrid>(width, height, cellSize, origin);
    _crowd = std::make_shared<RVOCrowd>();
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
