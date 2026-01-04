#include "rvo2_crowd.hpp"
#include <RVO.h>
#include <cmath>

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
        15.0f,   // neighborDist - how far to look for neighbors
        10,      // maxNeighbors - max neighbors to consider
        5.0f,    // timeHorizon - planning horizon for other agents
        5.0f,    // timeHorizonObst - planning horizon for obstacles
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
}
