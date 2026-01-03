#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <unordered_map>

// Forward declaration
class NavmeshGrid;

struct CrowdAgent {
    int Id = -1;
    glm::vec3 Position = glm::vec3(0);
    glm::vec3 Velocity = glm::vec3(0);
    glm::vec3 PreferredVelocity = glm::vec3(0);
    float Radius = 0.5f;
    float MaxSpeed = 5.0f;
    float MaxAcceleration = 10.0f;

    std::vector<glm::vec3> Path;
    size_t CurrentWaypoint = 0;
};

class RVOCrowd {
private:
    std::unordered_map<int, std::shared_ptr<CrowdAgent>> _agents;
    int _nextAgentId = 0;
    float _timeHorizon = 2.5f;

    [[nodiscard]] glm::vec3 ComputeRVOVelocity(const CrowdAgent& agent) const;
    [[nodiscard]] glm::vec3 ComputeAvoidanceVelocity(const CrowdAgent& agent, const std::vector<std::pair<float, const CrowdAgent*>>& nearbyAgents) const;

public:
    int AddAgent(const glm::vec3& position, float radius, float maxSpeed);
    void RemoveAgent(int agentId);

    void SetAgentPath(int agentId, const std::vector<glm::vec3>& path);
    void SetAgentPosition(int agentId, const glm::vec3& position);
    void UpdateAgents(float deltaTime);

    [[nodiscard]] std::shared_ptr<CrowdAgent> GetAgent(int agentId) const;
    [[nodiscard]] size_t GetAgentCount() const noexcept { return _agents.size(); }

    void Clear();
};
