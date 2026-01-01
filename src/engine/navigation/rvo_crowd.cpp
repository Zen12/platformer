#include "rvo_crowd.hpp"
#include <algorithm>
#include <cmath>

int RVOCrowd::AddAgent(const glm::vec3& position, float radius, float maxSpeed) {
    const int id = _nextAgentId++;

    auto agent = std::make_shared<CrowdAgent>();
    agent->Id = id;
    agent->Position = position;
    agent->Radius = radius;
    agent->MaxSpeed = maxSpeed;
    agent->Velocity = glm::vec3(0);
    agent->PreferredVelocity = glm::vec3(0);

    _agents[id] = agent;
    return id;
}

void RVOCrowd::RemoveAgent(int agentId) {
    _agents.erase(agentId);
}

void RVOCrowd::SetAgentPath(int agentId, const std::vector<glm::vec3>& path) {
    if (const auto it = _agents.find(agentId); it != _agents.end()) {
        it->second->Path = path;
        it->second->CurrentWaypoint = 0;
    }
}

void RVOCrowd::SetAgentPosition(int agentId, const glm::vec3& position) {
    if (const auto it = _agents.find(agentId); it != _agents.end()) {
        it->second->Position = position;
    }
}

std::shared_ptr<CrowdAgent> RVOCrowd::GetAgent(int agentId) const {
    if (const auto it = _agents.find(agentId); it != _agents.end()) {
        return it->second;
    }
    return nullptr;
}

void RVOCrowd::Clear() {
    _agents.clear();
    _nextAgentId = 0;
}

glm::vec3 RVOCrowd::ComputeAvoidanceVelocity(const CrowdAgent& agent, const std::vector<std::pair<float, const CrowdAgent*>>& nearbyAgents) const {
    glm::vec3 avoidanceVel = glm::vec3(0);

    for (const auto& [dist, other] : nearbyAgents) {
        const glm::vec3 relativePos = other->Position - agent.Position;
        const float distance = glm::length(relativePos);

        if (distance < 0.0001f) continue;

        const glm::vec3 relativeVel = agent.Velocity - other->Velocity;
        const float combinedRadius = agent.Radius + other->Radius;

        const glm::vec3 u = relativeVel - (relativePos / _timeHorizon);
        const float uLengthSq = glm::dot(u, u);

        if (uLengthSq < combinedRadius * combinedRadius) {
            const float uLength = std::sqrt(uLengthSq);
            if (uLength > 0.0001f) {
                const glm::vec3 avoidanceDir = u / uLength;
                const float avoidanceMag = combinedRadius - uLength;
                avoidanceVel += avoidanceDir * avoidanceMag * 0.5f;
            }
        } else if (distance < combinedRadius * 2.0f) {
            const glm::vec3 separationDir = -relativePos / distance;
            const float separationWeight = 1.0f - (distance / (combinedRadius * 2.0f));
            avoidanceVel += separationDir * separationWeight * 0.3f;
        }
    }

    return avoidanceVel;
}

glm::vec3 RVOCrowd::ComputeRVOVelocity(const CrowdAgent& agent, float deltaTime) const {
    glm::vec3 preferredVel = agent.PreferredVelocity;

    std::vector<std::pair<float, const CrowdAgent*>> nearbyAgents;
    for (const auto& [id, other] : _agents) {
        if (other->Id == agent.Id) continue;

        const float dist = glm::length(other->Position - agent.Position);
        const float searchRadius = _timeHorizon * agent.MaxSpeed;

        if (dist < searchRadius) {
            nearbyAgents.emplace_back(dist, other.get());
        }
    }

    std::sort(nearbyAgents.begin(), nearbyAgents.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    const glm::vec3 avoidanceVel = ComputeAvoidanceVelocity(agent, nearbyAgents);
    glm::vec3 newVel = preferredVel + avoidanceVel;

    const float speed = glm::length(newVel);
    if (speed > agent.MaxSpeed) {
        newVel = (newVel / speed) * agent.MaxSpeed;
    }

    const glm::vec3 velChange = newVel - agent.Velocity;
    const float velChangeLen = glm::length(velChange);
    const float maxVelChange = agent.MaxAcceleration * deltaTime;

    if (velChangeLen > maxVelChange) {
        return agent.Velocity + (velChange / velChangeLen) * maxVelChange;
    }

    return newVel;
}

void RVOCrowd::UpdateAgents(float deltaTime) {
    for (const auto& [id, agent] : _agents) {
        if (agent->Path.empty() || agent->CurrentWaypoint >= agent->Path.size()) {
            agent->PreferredVelocity = glm::vec3(0);
            continue;
        }

        const glm::vec3 target = agent->Path[agent->CurrentWaypoint];
        const glm::vec3 toTarget = target - agent->Position;
        const float dist = glm::length(toTarget);

        if (dist < agent->Radius * 0.5f) {
            agent->CurrentWaypoint++;
            if (agent->CurrentWaypoint >= agent->Path.size()) {
                agent->PreferredVelocity = glm::vec3(0);
                continue;
            }
        }

        if (dist > 0.0001f) {
            const glm::vec3 direction = toTarget / dist;
            const float targetSpeed = std::min(agent->MaxSpeed, dist / deltaTime);
            agent->PreferredVelocity = direction * targetSpeed;
        } else {
            agent->PreferredVelocity = glm::vec3(0);
        }
    }

    std::vector<std::pair<int, glm::vec3>> newVelocities;
    newVelocities.reserve(_agents.size());

    for (const auto& [id, agent] : _agents) {
        const glm::vec3 newVel = ComputeRVOVelocity(*agent, deltaTime);
        newVelocities.emplace_back(id, newVel);
    }

    for (const auto& [id, newVel] : newVelocities) {
        if (const auto it = _agents.find(id); it != _agents.end()) {
            it->second->Velocity = newVel;
            it->second->Position += newVel * deltaTime;
        }
    }
}
