#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <unordered_map>

namespace RVO {
    class RVOSimulator;
}

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

    // Hard separation enforcement
    void EnforceSeparation();

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
};
