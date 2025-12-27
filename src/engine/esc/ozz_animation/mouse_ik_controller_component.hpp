#pragma once
#include <glm/glm.hpp>

class MouseIKControllerComponent final {
public:
    bool Enabled = true;

    // Plane configuration for ray intersection
    glm::vec3 PlaneNormal = glm::vec3(0.0f, 1.0f, 0.0f);  // Ground plane
    float PlaneDistance = 0.0f;  // Distance from origin along normal

    // Offset applied to intersection point
    glm::vec3 TargetOffset = glm::vec3(0.0f, 0.0f, 0.0f);

    // Current world position (updated by system)
    glm::vec3 WorldPosition = glm::vec3(0.0f, 0.0f, 0.0f);
};
