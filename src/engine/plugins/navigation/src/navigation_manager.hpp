#pragma once
#include "grid_navmesh.hpp"
#include "rvo2_crowd.hpp"
#include <memory>
#include <cmath>
#include <algorithm>

struct ShootRaycastHit {
    glm::vec3 Point{0.0f};
    float Distance = 0.0f;
    int AgentId = -1;
    int Elevation = 0;
    bool Hit = false;
    bool HitAgent = false;
    bool HitTerrain = false;
};

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

    [[nodiscard]] NavmeshRaycastHit NavmeshRaycast(const glm::vec3& from, const glm::vec3& to) const {
        if (_navmesh) {
            return _navmesh->Raycast(from, to);
        }
        return {};
    }

    // Shoot raycast: traverses navmesh grid with elevation, checks for agents in each cell.
    // Stops at non-walkable cells, elevation barriers, or when hitting an agent on compatible elevation.
    // excludeAgentId: agent to skip (the shooter's own agent)
    [[nodiscard]] ShootRaycastHit ShootRaycast(const glm::vec3& from, const glm::vec3& to, int sourceElevation, int excludeAgentId = -1) const;

    void Clear();
};
