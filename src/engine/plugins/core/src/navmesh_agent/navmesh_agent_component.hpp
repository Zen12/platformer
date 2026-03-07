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

    // Current movement state (updated by NavmeshAgentSystem)
    float CurrentSpeed = 0.0f;  // Current velocity magnitude
    glm::vec3 CurrentVelocity{0.0f};  // Current velocity vector

    // Jump and gravity
    float VerticalVelocity = 0.0f;
    float Gravity = -20.0f;
    float JumpForce = 10.0f;
    float GroundY = 0.0f;  // Base Y level of navmesh (elevation 1)
    float ElevationHeight = 1.0f;  // Y increase per elevation level
    bool IsGrounded = true;
    bool IsJumping = false;
    bool JustLanded = false;  // True for one frame after landing to preserve air rotation
    float AirControlMultiplier = 0.8f;
    int CurrentElevation = 1;  // Current elevation level (1-3)

    // Manual waypoint following
    std::vector<glm::vec3> PathWaypoints;
    int CurrentWaypointIndex = 0;

    // Player-specific: ignore RVO2 velocity, move directly based on input
    // Agent still exists in crowd so AI agents avoid it, but player isn't pushed by AI
    bool IgnoreCrowdVelocity = false;

    // When true, skip rotation update (IK system controls rotation instead)
    bool ExternalRotationControl = false;

    explicit NavmeshAgentComponent(const NavmeshAgentComponentSerialization &serialization)
        : Radius(serialization.Radius), MaxSpeed(serialization.MaxSpeed),
          RotationSpeed(serialization.RotationSpeed), Enabled(serialization.Enabled),
          Gravity(serialization.Gravity), JumpForce(serialization.JumpForce),
          AirControlMultiplier(serialization.AirControlMultiplier),
          IgnoreCrowdVelocity(serialization.IgnoreCrowdVelocity) {}
};
