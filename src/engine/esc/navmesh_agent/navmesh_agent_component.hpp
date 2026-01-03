#pragma once
#include "navmesh_agent_component_serialization.hpp"
#include <glm/glm.hpp>
#include <vector>

class NavmeshAgentComponent final {
public:
    int CrowdAgentId = -1;
    float Radius = 0.5f;
    float MaxSpeed = 5.0f;
    float RotationSpeed = 10.0f;
    bool Enabled = true;
    glm::vec3 Destination = glm::vec3(0);
    bool HasDestination = false;
    bool DestinationChanged = false;  // Track if destination needs to be updated

    // Manual waypoint following
    std::vector<glm::vec3> PathWaypoints;
    int CurrentWaypointIndex = 0;

    explicit NavmeshAgentComponent(const NavmeshAgentComponentSerialization &serialization)
        : Radius(serialization.Radius), MaxSpeed(serialization.MaxSpeed),
          RotationSpeed(serialization.RotationSpeed), Enabled(serialization.Enabled) {}
};
