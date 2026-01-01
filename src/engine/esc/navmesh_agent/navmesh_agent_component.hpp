#pragma once
#include "navmesh_agent_component_serialization.hpp"
#include <glm/glm.hpp>

class NavmeshAgentComponent final {
public:
    int CrowdAgentId = -1;
    float Radius = 0.5f;
    float MaxSpeed = 5.0f;
    bool Enabled = true;
    glm::vec3 Destination = glm::vec3(0);
    bool HasDestination = false;

    explicit NavmeshAgentComponent(const NavmeshAgentComponentSerialization &serialization)
        : Radius(serialization.Radius), MaxSpeed(serialization.MaxSpeed), Enabled(serialization.Enabled) {}
};
