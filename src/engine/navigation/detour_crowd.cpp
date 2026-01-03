#include "detour_crowd.hpp"
#include "detour_navmesh.hpp"
#include <DetourCrowd.h>
#include <DetourNavMesh.h>
#include <cstring>
#include <iostream>

DetourCrowd::~DetourCrowd() {
    if (_crowd) {
        dtFreeCrowd(_crowd);
        _crowd = nullptr;
    }
}

bool DetourCrowd::Initialize(const std::shared_ptr<DetourNavmesh>& navmesh, int maxAgents) {
    if (!navmesh || !navmesh->GetNavMesh()) {
        return false;
    }

    _maxAgents = maxAgents;
    _crowd = dtAllocCrowd();
    if (!_crowd) {
        return false;
    }

    if (!_crowd->init(_maxAgents, 0.6f, navmesh->GetNavMesh())) {
        dtFreeCrowd(_crowd);
        _crowd = nullptr;
        return false;
    }

    // Setup crowd parameters
    dtObstacleAvoidanceParams params;
    memcpy(&params, _crowd->getObstacleAvoidanceParams(0), sizeof(dtObstacleAvoidanceParams));

    // Configure avoidance parameters for better behavior
    params.velBias = 0.4f;
    params.weightDesVel = 2.0f;
    params.weightCurVel = 0.75f;
    params.weightSide = 0.75f;
    params.weightToi = 2.5f;
    params.horizTime = 2.5f;
    params.gridSize = 33;
    params.adaptiveDivs = 7;
    params.adaptiveRings = 3;
    params.adaptiveDepth = 5;

    _crowd->setObstacleAvoidanceParams(0, &params);

    return true;
}

int DetourCrowd::AddAgent(const glm::vec3& position, float radius, float height, float maxSpeed) {
    if (!_crowd) {
        return -1;
    }

    dtCrowdAgentParams ap;
    memset(&ap, 0, sizeof(ap));
    ap.radius = radius;
    ap.height = height;
    ap.maxAcceleration = 20.0f;  // Increased from 8.0f for faster movement
    ap.maxSpeed = maxSpeed;
    ap.collisionQueryRange = radius * 12.0f;
    ap.pathOptimizationRange = radius * 30.0f;
    ap.updateFlags = 0;
    ap.updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
    ap.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
    ap.updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
    ap.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
    ap.updateFlags |= DT_CROWD_SEPARATION;
    ap.obstacleAvoidanceType = 0;
    ap.separationWeight = 0.5f;  // Reduced from 2.0f to reduce oscillation near destination

    const float pos[3] = { position.x, position.y, position.z };
    return _crowd->addAgent(pos, &ap);
}

void DetourCrowd::RemoveAgent(int agentId) {
    if (!_crowd || agentId < 0) {
        return;
    }

    _crowd->removeAgent(agentId);
}

void DetourCrowd::SetAgentTarget(int agentId, const glm::vec3& target) {
    if (!_crowd || agentId < 0) {
        return;
    }

    const dtCrowdAgent* agent = _crowd->getAgent(agentId);
    if (!agent || !agent->active) {
        return;
    }

    const dtQueryFilter* filter = _crowd->getFilter(0);
    const float* ext = _crowd->getQueryExtents();

    dtPolyRef targetRef = 0;
    float targetPos[3] = { target.x, target.y, target.z };
    float nearestPos[3] = { target.x, target.y, target.z };

    _crowd->getNavMeshQuery()->findNearestPoly(targetPos, ext, filter, &targetRef, nearestPos);

    if (targetRef) {
        _crowd->requestMoveTarget(agentId, targetRef, nearestPos);
    }
}

void DetourCrowd::SetAgentPath(int agentId, const std::vector<glm::vec3>& path) {
    if (!_crowd || agentId < 0 || path.empty()) {
        return;
    }

    // Set target to the last point in the path
    SetAgentTarget(agentId, path.back());
}

void DetourCrowd::ResetAgentTarget(int agentId) {
    if (!_crowd || agentId < 0) {
        return;
    }

    _crowd->resetMoveTarget(agentId);
}

glm::vec3 DetourCrowd::GetAgentPosition(int agentId) const {
    if (!_crowd || agentId < 0) {
        return glm::vec3(0.0f);
    }

    const dtCrowdAgent* agent = _crowd->getAgent(agentId);
    if (!agent || !agent->active) {
        return glm::vec3(0.0f);
    }

    return glm::vec3(agent->npos[0], agent->npos[1], agent->npos[2]);
}

glm::vec3 DetourCrowd::GetAgentVelocity(int agentId) const {
    if (!_crowd || agentId < 0) {
        return glm::vec3(0.0f);
    }

    const dtCrowdAgent* agent = _crowd->getAgent(agentId);
    if (!agent || !agent->active) {
        return glm::vec3(0.0f);
    }

    return glm::vec3(agent->vel[0], agent->vel[1], agent->vel[2]);
}

bool DetourCrowd::IsAgentActive(int agentId) const {
    if (!_crowd || agentId < 0) {
        return false;
    }

    const dtCrowdAgent* agent = _crowd->getAgent(agentId);
    return agent && agent->active;
}

void DetourCrowd::Update(float deltaTime) {
    if (!_crowd) {
        return;
    }

    _crowd->update(deltaTime, nullptr);
}

void DetourCrowd::Clear() {
    if (!_crowd) {
        return;
    }

    // Remove all agents
    for (int i = 0; i < _crowd->getAgentCount(); ++i) {
        const dtCrowdAgent* agent = _crowd->getAgent(i);
        if (agent && agent->active) {
            _crowd->removeAgent(i);
        }
    }
}
