#include "navigation_manager.hpp"
#include <unordered_set>

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

ShootRaycastHit NavigationManager::ShootRaycast(const glm::vec3& from, const glm::vec3& to, int sourceElevation, int excludeAgentId) const {
    ShootRaycastHit result;
    if (!_navmesh || !_crowd) return result;

    const auto& grid = _navmesh->GetGrid();
    const float cellSize = _navmesh->GetCellSize();
    const auto& origin = _navmesh->GetOrigin();
    const int width = _navmesh->GetWidth();
    const int height = _navmesh->GetHeight();

    auto worldToGrid = [&](const glm::vec3& pos, int& outX, int& outZ) {
        outX = static_cast<int>(std::floor((pos.x - origin.x) / cellSize));
        outZ = static_cast<int>(std::floor((pos.z - origin.z) / cellSize));
    };

    auto gridToWorld = [&](int x, int z) -> glm::vec3 {
        return {origin.x + (static_cast<float>(x) + 0.5f) * cellSize,
                origin.y,
                origin.z + (static_cast<float>(z) + 0.5f) * cellSize};
    };

    auto isValid = [&](int x, int z) { return x >= 0 && x < width && z >= 0 && z < height; };
    auto getElev = [&](int x, int z) -> int {
        if (!isValid(x, z)) return 0;
        return grid[z][x];
    };
    auto isWalkable = [&](int x, int z) { return isValid(x, z) && grid[z][x] != 0; };
    auto canTraverse = [](int a, int b) { return a != 0 && b != 0 && std::abs(a - b) <= 1; };

    int x0, z0, x1, z1;
    worldToGrid(from, x0, z0);
    worldToGrid(to, x1, z1);

    if (!isWalkable(x0, z0)) return result;

    int rayElevation = sourceElevation;

    const int dx = std::abs(x1 - x0);
    const int dz = std::abs(z1 - z0);
    const int sx = x0 < x1 ? 1 : -1;
    const int sz = z0 < z1 ? 1 : -1;
    int err = dx - dz;

    std::unordered_set<int> checkedAgents;

    while (true) {
        // Check for agents in current cell and neighbors
        for (int nz = z0 - 1; nz <= z0 + 1; ++nz) {
            for (int nx = x0 - 1; nx <= x0 + 1; ++nx) {
                if (!isValid(nx, nz)) continue;
                for (int agentId : _crowd->GetAgentsInCell(nx, nz)) {
                    if (agentId == excludeAgentId) continue;
                    if (checkedAgents.count(agentId)) continue;
                    checkedAgents.insert(agentId);

                    if (!_crowd->IsAgentActive(agentId)) continue;

                    // Check agent is on compatible elevation
                    const glm::vec3 agentPos = _crowd->GetAgentPosition(agentId);
                    const int agentElev = getElev(nx, nz);
                    if (!canTraverse(rayElevation, agentElev)) continue;

                    // Check if agent is close to the ray line in XZ
                    const glm::vec2 rayStart(from.x, from.z);
                    const glm::vec2 rayEnd(to.x, to.z);
                    const glm::vec2 rayDir = rayEnd - rayStart;
                    const float rayLen = glm::length(rayDir);
                    if (rayLen < 0.001f) continue;

                    const glm::vec2 rayDirNorm = rayDir / rayLen;
                    const glm::vec2 toAgent(agentPos.x - from.x, agentPos.z - from.z);

                    const float proj = glm::dot(toAgent, rayDirNorm);
                    if (proj < 0.0f) continue;

                    const float perpDist = std::abs(toAgent.x * rayDirNorm.y - toAgent.y * rayDirNorm.x);
                    const float hitRadius = _crowd->GetAgentRadius(agentId) + cellSize * 0.5f;

                    if (perpDist <= hitRadius) {
                        result.Hit = true;
                        result.HitAgent = true;
                        result.AgentId = agentId;
                        result.Point = agentPos;
                        result.Distance = proj;
                        result.Elevation = agentElev;
                        return result;
                    }
                }
            }
        }

        if (x0 == x1 && z0 == z1) break;

        const int e2 = 2 * err;
        int nextX = x0, nextZ = z0;
        if (e2 > -dz) {
            err -= dz;
            nextX += sx;
        }
        if (e2 < dx) {
            err += dx;
            nextZ += sz;
        }

        x0 = nextX;
        z0 = nextZ;

        // Check terrain: non-walkable or elevation barrier
        const int cellElev = getElev(x0, z0);
        if (!isWalkable(x0, z0) || !canTraverse(rayElevation, cellElev)) {
            result.Hit = true;
            result.HitTerrain = true;
            result.Point = gridToWorld(x0, z0);
            result.Distance = glm::length(glm::vec2(result.Point.x - from.x, result.Point.z - from.z));
            result.Elevation = cellElev;
            return result;
        }

        rayElevation = cellElev;
    }

    // No hit — set final position
    result.Point = to;
    result.Distance = glm::length(glm::vec2(to.x - from.x, to.z - from.z));
    return result;
}

void NavigationManager::Clear() {
    if (_crowd) {
        _crowd->Clear();
    }
    _navmesh.reset();
    _crowd.reset();
}
