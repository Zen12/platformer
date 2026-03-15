#include "rvo2_crowd.hpp"
#include <RVO.h>
#include <cmath>
#include <unordered_set>

RVO2Crowd::RVO2Crowd() = default;

RVO2Crowd::~RVO2Crowd() = default;

bool RVO2Crowd::Initialize(int maxAgents) {
    _maxAgents = maxAgents;
    _simulator = std::make_unique<RVO::RVOSimulator>();

    // Set time step (will be overridden in Update)
    _simulator->setTimeStep(1.0f / 60.0f);

    // Set default agent parameters
    // neighborDist, maxNeighbors, timeHorizon, timeHorizonObst, radius, maxSpeed
    _simulator->setAgentDefaults(
        5.0f,    // neighborDist - how far to look for neighbors (reduced from 15)
        10,      // maxNeighbors - max neighbors to consider
        1.0f,    // timeHorizon - planning horizon for other agents (reduced from 5 - less early slowdown)
        1.0f,    // timeHorizonObst - planning horizon for obstacles (reduced from 5)
        0.5f,    // radius - default agent radius
        5.0f     // maxSpeed - default max speed
    );

    return true;
}

int RVO2Crowd::AddAgent(const glm::vec3& position, float radius, [[maybe_unused]] float height, float maxSpeed) {
    if (!_simulator) {
        return -1;
    }

    // RVO2 is 2D, so we ignore height and use x,z plane
    RVO::Vector2 pos(position.x, position.z);

    // Add agent with custom parameters
    size_t rvoIndex = _simulator->addAgent(pos);

    // Set agent-specific parameters
    _simulator->setAgentRadius(rvoIndex, radius);
    _simulator->setAgentMaxSpeed(rvoIndex, maxSpeed);

    // Generate our agent ID
    int agentId = _nextAgentId++;

    // Store mappings
    _agentIdToIndex[agentId] = rvoIndex;
    _indexToAgentId[rvoIndex] = agentId;

    // Store agent data
    AgentData data;
    data.Position = position;
    data.Radius = radius;
    data.MaxSpeed = maxSpeed;
    data.Active = true;
    _agentData[agentId] = data;

    return agentId;
}

void RVO2Crowd::RemoveAgent(int agentId) {
    auto it = _agentIdToIndex.find(agentId);
    if (it == _agentIdToIndex.end()) {
        return;
    }

    // Mark as inactive (RVO2 doesn't support removing agents)
    // We'll set velocity to 0 and ignore it
    _agentData[agentId].Active = false;
    _simulator->setAgentMaxSpeed(it->second, 0.0f);
}

void RVO2Crowd::SetAgentTarget(int agentId, const glm::vec3& target) {
    auto it = _agentData.find(agentId);
    if (it == _agentData.end()) {
        return;
    }

    it->second.Target = target;
    it->second.HasTarget = true;
}

void RVO2Crowd::ResetAgentTarget(int agentId) {
    auto it = _agentData.find(agentId);
    if (it == _agentData.end()) {
        return;
    }

    it->second.HasTarget = false;
}

glm::vec3 RVO2Crowd::GetAgentPosition(int agentId) const {
    auto it = _agentData.find(agentId);
    if (it == _agentData.end()) {
        return glm::vec3(0.0f);
    }
    return it->second.Position;
}

glm::vec3 RVO2Crowd::GetAgentVelocity(int agentId) const {
    auto it = _agentData.find(agentId);
    if (it == _agentData.end()) {
        return glm::vec3(0.0f);
    }
    return it->second.Velocity;
}

bool RVO2Crowd::IsAgentActive(int agentId) const {
    auto it = _agentData.find(agentId);
    if (it == _agentData.end()) {
        return false;
    }
    return it->second.Active;
}

float RVO2Crowd::GetAgentRadius(int agentId) const {
    auto it = _agentData.find(agentId);
    if (it == _agentData.end()) {
        return 0.5f;
    }
    return it->second.Radius;
}

void RVO2Crowd::SetAgentPosition(int agentId, const glm::vec3& position) {
    auto dataIt = _agentData.find(agentId);
    if (dataIt == _agentData.end()) {
        return;
    }

    auto indexIt = _agentIdToIndex.find(agentId);
    if (indexIt == _agentIdToIndex.end()) {
        return;
    }

    // Update our stored data
    dataIt->second.Position = position;

    // Update RVO2 simulator position
    _simulator->setAgentPosition(indexIt->second, RVO::Vector2(position.x, position.z));
}

void RVO2Crowd::Update(float deltaTime) {
    if (!_simulator || deltaTime <= 0.0f) {
        return;
    }

    // Set time step for this frame
    _simulator->setTimeStep(deltaTime);

    // Set preferred velocities for all agents
    for (auto& [agentId, data] : _agentData) {
        if (!data.Active) {
            continue;
        }

        auto indexIt = _agentIdToIndex.find(agentId);
        if (indexIt == _agentIdToIndex.end()) {
            continue;
        }

        size_t rvoIndex = indexIt->second;

        if (data.HasTarget) {
            // Calculate direction to target (2D)
            glm::vec2 toTarget(data.Target.x - data.Position.x, data.Target.z - data.Position.z);
            float dist = glm::length(toTarget);

            if (dist > 0.1f) {
                // Set preferred velocity towards target
                glm::vec2 prefVel = (toTarget / dist) * data.MaxSpeed;
                _simulator->setAgentPrefVelocity(rvoIndex, RVO::Vector2(prefVel.x, prefVel.y));
            } else {
                // Close enough, stop
                _simulator->setAgentPrefVelocity(rvoIndex, RVO::Vector2(0.0f, 0.0f));
            }
        } else {
            // No target, stop
            _simulator->setAgentPrefVelocity(rvoIndex, RVO::Vector2(0.0f, 0.0f));
        }
    }

    // Run RVO2 simulation step
    _simulator->doStep();

    // Update agent positions from RVO2
    for (auto& [agentId, data] : _agentData) {
        if (!data.Active) {
            continue;
        }

        auto indexIt = _agentIdToIndex.find(agentId);
        if (indexIt == _agentIdToIndex.end()) {
            continue;
        }

        size_t rvoIndex = indexIt->second;

        // Get new position and velocity from RVO2
        const RVO::Vector2& pos = _simulator->getAgentPosition(rvoIndex);
        const RVO::Vector2& vel = _simulator->getAgentVelocity(rvoIndex);

        // Update our stored data (keep Y from before)
        data.Position.x = pos.x();
        data.Position.z = pos.y();  // RVO2 y -> our z
        data.Velocity = glm::vec3(vel.x(), 0.0f, vel.y());
    }

    // Hard separation enforcement - prevent any overlapping
    EnforceSeparation();

    // Rebuild spatial grid for efficient raycasting
    RebuildSpatialGrid();
}

void RVO2Crowd::EnforceSeparation() {
    // Collect active agents
    std::vector<int> activeAgents;
    activeAgents.reserve(_agentData.size());
    for (const auto& [agentId, data] : _agentData) {
        if (data.Active) {
            activeAgents.push_back(agentId);
        }
    }

    // Push apart overlapping agents
    constexpr int maxIterations = 3;
    for (int iter = 0; iter < maxIterations; ++iter) {
        bool anyOverlap = false;

        for (size_t i = 0; i < activeAgents.size(); ++i) {
            auto& dataA = _agentData[activeAgents[i]];

            for (size_t j = i + 1; j < activeAgents.size(); ++j) {
                auto& dataB = _agentData[activeAgents[j]];

                // Calculate distance between agents (2D)
                const float dx = dataB.Position.x - dataA.Position.x;
                const float dz = dataB.Position.z - dataA.Position.z;
                const float distSq = dx * dx + dz * dz;

                const float minDist = dataA.Radius + dataB.Radius;
                const float minDistSq = minDist * minDist;

                if (distSq < minDistSq && distSq > 0.0001f) {
                    anyOverlap = true;
                    const float dist = std::sqrt(distSq);
                    const float overlap = minDist - dist;

                    // Push each agent away by half the overlap
                    const float pushDist = overlap * 0.5f + 0.01f;  // Small extra to ensure separation
                    const float nx = dx / dist;
                    const float nz = dz / dist;

                    dataA.Position.x -= nx * pushDist;
                    dataA.Position.z -= nz * pushDist;
                    dataB.Position.x += nx * pushDist;
                    dataB.Position.z += nz * pushDist;
                }
            }
        }

        if (!anyOverlap) {
            break;
        }
    }

    // Sync corrected positions back to RVO2 simulator
    for (int agentId : activeAgents) {
        auto indexIt = _agentIdToIndex.find(agentId);
        if (indexIt != _agentIdToIndex.end()) {
            const auto& data = _agentData[agentId];
            _simulator->setAgentPosition(indexIt->second, RVO::Vector2(data.Position.x, data.Position.z));
        }
    }
}

void RVO2Crowd::Clear() {
    _agentIdToIndex.clear();
    _indexToAgentId.clear();
    _agentData.clear();
    _nextAgentId = 0;

    // Recreate simulator
    if (_simulator) {
        _simulator = std::make_unique<RVO::RVOSimulator>();
        _simulator->setTimeStep(1.0f / 60.0f);
        _simulator->setAgentDefaults(15.0f, 10, 5.0f, 5.0f, 0.5f, 5.0f);
    }

    // Clear spatial grid
    for (auto& row : _agentGrid) {
        for (auto& cell : row) {
            cell.clear();
        }
    }
}

// ============================================================================
// Spatial Grid Implementation
// ============================================================================

void RVO2Crowd::InitializeSpatialGrid(int width, int height, float cellSize, const glm::vec3& origin) {
    _gridWidth = width;
    _gridHeight = height;
    _gridCellSize = cellSize;
    _gridOrigin = origin;

    // Allocate grid
    _agentGrid.resize(height);
    for (int z = 0; z < height; ++z) {
        _agentGrid[z].resize(width);
    }

    _gridInitialized = true;
}

void RVO2Crowd::WorldToGrid(const glm::vec3& pos, int& outX, int& outZ) const {
    outX = static_cast<int>(std::floor((pos.x - _gridOrigin.x) / _gridCellSize));
    outZ = static_cast<int>(std::floor((pos.z - _gridOrigin.z) / _gridCellSize));
}

bool RVO2Crowd::IsValidCell(int x, int z) const {
    return x >= 0 && x < _gridWidth && z >= 0 && z < _gridHeight;
}

void RVO2Crowd::RebuildSpatialGrid() {
    if (!_gridInitialized) return;

    // Clear all cells
    for (auto& row : _agentGrid) {
        for (auto& cell : row) {
            cell.clear();
        }
    }

    // Insert all active agents into their cells
    for (const auto& [agentId, data] : _agentData) {
        if (!data.Active) continue;

        int x, z;
        WorldToGrid(data.Position, x, z);

        if (IsValidCell(x, z)) {
            _agentGrid[z][x].push_back(agentId);
        }
    }
}

static const std::vector<int> _emptyCell;

const std::vector<int>& RVO2Crowd::GetAgentsInCell(int x, int z) const {
    if (!_gridInitialized || !IsValidCell(x, z)) {
        return _emptyCell;
    }
    return _agentGrid[z][x];
}

float RVO2Crowd::RaySphereIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                     const glm::vec3& sphereCenter, float sphereRadius) const {
    // Vector from ray origin to sphere center
    glm::vec3 oc = rayOrigin - sphereCenter;

    // Quadratic coefficients: at^2 + bt + c = 0
    float a = glm::dot(rayDir, rayDir);
    float b = 2.0f * glm::dot(oc, rayDir);
    float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;

    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f) {
        return -1.0f;  // No intersection
    }

    // Find the nearest intersection point
    float sqrtDisc = std::sqrt(discriminant);
    float t1 = (-b - sqrtDisc) / (2.0f * a);
    float t2 = (-b + sqrtDisc) / (2.0f * a);

    // Return the closest positive t
    if (t1 > 0.0f) return t1;
    if (t2 > 0.0f) return t2;
    return -1.0f;  // Both intersections behind ray origin
}

RaycastHit RVO2Crowd::Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const {
    RaycastHit result;

    if (!_gridInitialized) {
        return result;
    }

    // Normalize direction
    glm::vec3 dir = glm::normalize(direction);

    // Convert ray start/end to grid coordinates
    int x0, z0, x1, z1;
    WorldToGrid(origin, x0, z0);
    glm::vec3 endPoint = origin + dir * maxDistance;
    WorldToGrid(endPoint, x1, z1);

    // Bresenham line traversal through grid cells
    const int dx = std::abs(x1 - x0);
    const int dz = std::abs(z1 - z0);
    const int sx = x0 < x1 ? 1 : -1;
    const int sz = z0 < z1 ? 1 : -1;
    int err = dx - dz;

    float closestDist = maxDistance + 1.0f;
    int closestAgent = -1;
    glm::vec3 closestHitPoint;

    int x = x0, z = z0;

    // Track checked agents to avoid double-checking
    std::unordered_set<int> checkedAgents;

    while (true) {
        // Check agents in current cell and neighboring cells (10x10 area)
        for (int nz = z - 5; nz <= z + 5; ++nz) {
            for (int nx = x - 5; nx <= x + 5; ++nx) {
                if (!IsValidCell(nx, nz)) continue;
                for (int agentId : _agentGrid[nz][nx]) {
                    // Skip already checked agents
                    if (checkedAgents.count(agentId)) continue;
                    checkedAgents.insert(agentId);

                    const auto& data = _agentData.at(agentId);
                    if (!data.Active) continue;

                    // Use agent position at ray height for 2D-style hit detection
                    glm::vec3 agentPos = data.Position;
                    agentPos.y = origin.y;  // Project to same Y plane as ray

                    // Ray-sphere intersection
                    float t = RaySphereIntersect(origin, dir, agentPos, data.Radius);

                    // Skip hits too close (self-hit) - minimum distance is 1.0
                    if (t > 1.0f && t < closestDist && t <= maxDistance) {
                        closestDist = t;
                        closestAgent = agentId;
                        closestHitPoint = origin + dir * t;
                    }
                }
            }
        }

        // Check if we've reached the end
        if (x == x1 && z == z1) break;

        // Move to next cell
        const int e2 = 2 * err;
        if (e2 > -dz) {
            err -= dz;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            z += sz;
        }
    }

    if (closestAgent != -1) {
        result.Hit = true;
        result.AgentId = closestAgent;
        result.HitPoint = closestHitPoint;
        result.Distance = closestDist;
    }

    return result;
}

void RVO2Crowd::AddObstacle(const std::vector<glm::vec3>& vertices) {
    if (!_simulator || vertices.size() < 3) {
        return;
    }

    // Convert to RVO2 2D vertices (use x,z plane)
    std::vector<RVO::Vector2> rvoVertices;
    rvoVertices.reserve(vertices.size());
    for (const auto& v : vertices) {
        rvoVertices.emplace_back(v.x, v.z);
    }

    _simulator->addObstacle(rvoVertices);
}

void RVO2Crowd::ProcessObstacles() {
    if (_simulator) {
        _simulator->processObstacles();
    }
}

glm::vec3 RVO2Crowd::ResolveCollision(const glm::vec3& position, float radius, int excludeAgentId) const {
    glm::vec3 resolvedPos = position;

    // Iterate multiple times to handle multiple overlapping agents
    constexpr int maxIterations = 3;
    for (int iter = 0; iter < maxIterations; ++iter) {
        bool anyCollision = false;

        for (const auto& [agentId, data] : _agentData) {
            if (!data.Active || agentId == excludeAgentId) continue;

            // Calculate distance between agent and position (2D)
            const float dx = resolvedPos.x - data.Position.x;
            const float dz = resolvedPos.z - data.Position.z;
            const float distSq = dx * dx + dz * dz;

            const float minDist = radius + data.Radius;
            const float minDistSq = minDist * minDist;

            if (distSq < minDistSq && distSq > 0.0001f) {
                anyCollision = true;
                const float dist = std::sqrt(distSq);
                const float overlap = minDist - dist;

                // Push position away from agent
                const float nx = dx / dist;
                const float nz = dz / dist;

                resolvedPos.x += nx * (overlap + 0.01f);
                resolvedPos.z += nz * (overlap + 0.01f);
            }
        }

        if (!anyCollision) break;
    }

    return resolvedPos;
}
