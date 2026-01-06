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

        // Add navmesh boundary edges as RVO2 obstacles so agents avoid edges
        // RVO2 requires polygons with at least 3 vertices, so we make thin rectangles
        const auto boundaryEdges = _navmesh->GetBoundaryEdges();
        constexpr float thickness = 0.05f;  // Thin obstacle thickness
        for (const auto& edge : boundaryEdges) {
            // Calculate edge direction and perpendicular
            glm::vec3 dir = edge.second - edge.first;
            const float len = glm::length(glm::vec2(dir.x, dir.z));
            if (len < 0.001f) continue;

            dir = dir / len;
            glm::vec3 perp(-dir.z, 0.0f, dir.x);  // Perpendicular in XZ plane

            // Create thin rectangle (CCW winding) - obstacle is on the left side of edge
            std::vector<glm::vec3> obstacle = {
                edge.first,
                edge.first - perp * thickness,
                edge.second - perp * thickness,
                edge.second
            };
            _crowd->AddObstacle(obstacle);
        }
        _crowd->ProcessObstacles();
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
