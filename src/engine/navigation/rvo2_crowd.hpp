#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <unordered_map>

namespace RVO {
    class RVOSimulator;
}

// Raycast hit result
struct RaycastHit {
    int AgentId = -1;
    glm::vec3 HitPoint{0.0f};
    float Distance = 0.0f;
    bool Hit = false;
};

class RVO2Crowd {
private:
    std::unique_ptr<RVO::RVOSimulator> _simulator;
    int _maxAgents = 2000;

    // Map from our agent ID to RVO2 agent index
    std::unordered_map<int, size_t> _agentIdToIndex;
    std::unordered_map<size_t, int> _indexToAgentId;
    int _nextAgentId = 0;

    // Store agent data for external access
    struct AgentData {
        glm::vec3 Position{0.0f};
        glm::vec3 Velocity{0.0f};
        glm::vec3 Target{0.0f};
        float Radius = 0.5f;
        float MaxSpeed = 5.0f;
        bool HasTarget = false;
        bool Active = true;
    };
    std::unordered_map<int, AgentData> _agentData;

    // Spatial grid for O(1) agent lookup
    float _gridCellSize = 1.0f;
    glm::vec3 _gridOrigin{0.0f};
    int _gridWidth = 0;
    int _gridHeight = 0;
    std::vector<std::vector<std::vector<int>>> _agentGrid;  // [z][x] -> list of agent IDs
    bool _gridInitialized = false;

    // Hard separation enforcement
    void EnforceSeparation();

    // Spatial grid helpers
    void WorldToGrid(const glm::vec3& pos, int& outX, int& outZ) const;
    bool IsValidCell(int x, int z) const;
    void RebuildSpatialGrid();

    // Ray-sphere intersection test (returns distance or -1 if no hit)
    float RaySphereIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                             const glm::vec3& sphereCenter, float sphereRadius) const;

public:
    RVO2Crowd();
    ~RVO2Crowd();

    // Delete copy constructor and assignment operator
    RVO2Crowd(const RVO2Crowd&) = delete;
    RVO2Crowd& operator=(const RVO2Crowd&) = delete;

    // Initialize crowd
    bool Initialize(int maxAgents = 2000);

    // Agent management
    int AddAgent(const glm::vec3& position, float radius = 0.5f, float height = 2.0f, float maxSpeed = 5.0f);
    void RemoveAgent(int agentId);

    // Agent control
    void SetAgentTarget(int agentId, const glm::vec3& target);
    void ResetAgentTarget(int agentId);

    // Agent queries
    glm::vec3 GetAgentPosition(int agentId) const;
    glm::vec3 GetAgentVelocity(int agentId) const;
    bool IsAgentActive(int agentId) const;
    float GetAgentRadius(int agentId) const;

    // Position correction (for navmesh clamping)
    void SetAgentPosition(int agentId, const glm::vec3& position);

    // Update
    void Update(float deltaTime);

    // Clear all agents
    void Clear();

    // Getters
    int GetMaxAgents() const noexcept { return _maxAgents; }
    size_t GetAgentCount() const noexcept { return _agentData.size(); }

    // Spatial grid - must be initialized before Raycast works
    void InitializeSpatialGrid(int width, int height, float cellSize, const glm::vec3& origin);

    // Add obstacles (navmesh boundaries) - vertices form a closed polygon in CCW order
    void AddObstacle(const std::vector<glm::vec3>& vertices);
    void ProcessObstacles();  // Must be called after all obstacles are added

    // Get agents in a specific grid cell (O(1) lookup)
    const std::vector<int>& GetAgentsInCell(int x, int z) const;

    // Raycast against agents using spatial grid (O(ray_cells * agents_per_cell))
    // Returns hit info with agent ID, hit point, and distance
    RaycastHit Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const;

    // Check if a position (with radius) collides with any agent except excludeAgentId
    // Returns the closest valid position (pushed out of collision)
    glm::vec3 ResolveCollision(const glm::vec3& position, float radius, int excludeAgentId) const;
};
