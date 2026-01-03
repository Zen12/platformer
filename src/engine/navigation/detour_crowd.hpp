#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class dtCrowd;
class DetourNavmesh;

class DetourCrowd {
private:
    dtCrowd* _crowd = nullptr;
    int _maxAgents = 2000;

public:
    DetourCrowd() = default;
    ~DetourCrowd();

    // Delete copy constructor and assignment operator
    DetourCrowd(const DetourCrowd&) = delete;
    DetourCrowd& operator=(const DetourCrowd&) = delete;

    // Initialize crowd with navmesh
    bool Initialize(const std::shared_ptr<DetourNavmesh>& navmesh, int maxAgents = 2000);

    // Agent management
    int AddAgent(const glm::vec3& position, float radius = 0.6f, float height = 2.0f, float maxSpeed = 3.5f);
    void RemoveAgent(int agentId);

    // Agent control
    void SetAgentTarget(int agentId, const glm::vec3& target);
    void SetAgentPath(int agentId, const std::vector<glm::vec3>& path);
    void ResetAgentTarget(int agentId);

    // Agent queries
    glm::vec3 GetAgentPosition(int agentId) const;
    glm::vec3 GetAgentVelocity(int agentId) const;
    bool IsAgentActive(int agentId) const;

    // Update
    void Update(float deltaTime);

    // Clear all agents
    void Clear();

    // Getters
    dtCrowd* GetCrowd() const noexcept { return _crowd; }
    int GetMaxAgents() const noexcept { return _maxAgents; }
};
