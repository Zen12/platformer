#pragma once
#include "../esc_core.hpp"
#include "navmesh_agent_component.hpp"
#include "../transform/transform_component.hpp"
#include "../time/time_component.hpp"
#include "../../navigation/navigation_manager.hpp"
#include "../../scene/scene.hpp"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

class NavmeshAgentSystem final : public ISystemView<NavmeshAgentComponent, TransformComponentV2> {
private:
    decltype(std::declval<entt::registry&>().view<DeltaTimeComponent>()) _timeView;
    std::shared_ptr<NavigationManager> _navigationManager;
    std::weak_ptr<Scene> _scene;

public:
    NavmeshAgentSystem(
        TypeView view,
        decltype(std::declval<entt::registry&>().view<DeltaTimeComponent>()) timeView,
        std::shared_ptr<NavigationManager> navigationManager,
        const std::weak_ptr<Scene>& scene)
        : ISystemView(view), _timeView(timeView), _navigationManager(std::move(navigationManager)), _scene(scene) {}

    void OnTick() override {
        if (!_navigationManager) {
            return;
        }

        float deltaTime = 0.0f;
        for (auto [entity, time] : _timeView.each()) {
            deltaTime = time.Delta;
        }

        auto navmesh = _navigationManager->GetNavmesh();
        auto crowd = _navigationManager->GetCrowd();

        if (!navmesh || !crowd) {
            return;
        }

        for (auto [entity, agent, transform] : View.each()) {
            if (!agent.Enabled) continue;

            glm::vec3 currentPos = transform.GetPosition();

            // Initialize GroundY from navmesh origin on first tick
            if (agent.GroundY == 0.0f) {
                agent.GroundY = navmesh->GetOrigin().y;
            }

            // Create crowd agent if needed
            if (agent.CrowdAgentId == -1) {
                agent.CrowdAgentId = crowd->AddAgent(currentPos, agent.Radius, 2.0f, agent.MaxSpeed);
            }

            // Check if agent is still active
            if (!crowd->IsAgentActive(agent.CrowdAgentId)) {
                continue;
            }

            const bool isOverNavmesh = navmesh->IsWalkable(currentPos);

            // Handle airborne state (jumping or fell off navmesh)
            if (!agent.IsGrounded) {
                // Apply gravity
                agent.VerticalVelocity += agent.Gravity * deltaTime;
                currentPos.y += agent.VerticalVelocity * deltaTime;

                // Check for landing on navmesh
                if (currentPos.y <= agent.GroundY && isOverNavmesh) {
                    // Land on navmesh
                    currentPos.y = agent.GroundY;
                    agent.IsGrounded = true;
                    agent.IsJumping = false;
                    agent.VerticalVelocity = 0.0f;
                    agent.JustLanded = true;  // Skip rotation update for one frame

                    // Sync crowd agent position
                    crowd->SetAgentPosition(agent.CrowdAgentId, currentPos);

                    // Clear navigation path but preserve velocity for smooth landing
                    agent.PathWaypoints.clear();
                    agent.CurrentWaypointIndex = 0;
                    // Don't clear HasDestination or velocity - let the grounded logic handle it smoothly
                }
                // Check for void fall (respawn)
                else if (currentPos.y < -10.0f) {
                    // Respawn at nearest walkable point, but higher so player falls down
                    const glm::vec3 respawn = navmesh->FindNearestPoint(glm::vec3(currentPos.x, agent.GroundY, currentPos.z));
                    currentPos = respawn;
                    currentPos.y = agent.GroundY + 3.0f;  // Spawn 3 units above ground
                    agent.IsGrounded = false;  // Will fall down
                    agent.IsJumping = false;
                    agent.VerticalVelocity = 0.0f;
                    crowd->SetAgentPosition(agent.CrowdAgentId, glm::vec3(currentPos.x, agent.GroundY, currentPos.z));
                }

                transform.SetPosition(currentPos);

                // Update velocity for animation (use horizontal velocity from air movement)
                agent.CurrentSpeed = glm::length(glm::vec2(agent.CurrentVelocity.x, agent.CurrentVelocity.z));

                // Rotate towards movement direction while airborne
                if (agent.CurrentSpeed > 0.1f) {
                    const float targetYaw = glm::degrees(std::atan2(agent.CurrentVelocity.x, agent.CurrentVelocity.z));
                    auto rotation = transform.GetEulerRotation();

                    float angleDiff = targetYaw - rotation.y;
                    while (angleDiff > 180.0f) angleDiff -= 360.0f;
                    while (angleDiff < -180.0f) angleDiff += 360.0f;

                    rotation.y += angleDiff * std::min(1.0f, agent.RotationSpeed * deltaTime);
                    transform.SetEulerRotation(rotation);
                }
            }
            // Handle grounded state
            else {
                // Check if walked off navmesh edge
                if (!isOverNavmesh) {
                    agent.IsGrounded = false;
                    agent.VerticalVelocity = 0.0f;  // Start falling
                    // Don't update position this frame, let gravity handle it next frame
                }
                else {
                    // Normal navmesh navigation
                    // Set destination when it changes
                    if (agent.HasDestination && agent.DestinationChanged) {
                        // Clamp destination to walkable area
                        glm::vec3 clampedDest = agent.Destination;
                        if (!navmesh->IsWalkable(clampedDest)) {
                            clampedDest = navmesh->FindNearestPoint(clampedDest);
                        }

                        crowd->SetAgentTarget(agent.CrowdAgentId, clampedDest);
                        agent.DestinationChanged = false;

                        // Cache path for visualization
                        agent.PathWaypoints = navmesh->FindPath(currentPos, clampedDest);
                        agent.CurrentWaypointIndex = 0;
                    }

                    glm::vec3 agentPos;
                    glm::vec3 velocity;

                    // Only use crowd position when actively navigating
                    if (agent.HasDestination) {
                        // Update transform from crowd
                        agentPos = crowd->GetAgentPosition(agent.CrowdAgentId);

                        // Stay on navmesh - find nearest walkable if off
                        if (!navmesh->IsWalkable(agentPos)) {
                            agentPos = navmesh->FindNearestPoint(agentPos);
                            crowd->SetAgentPosition(agent.CrowdAgentId, agentPos);
                        }

                        // Keep Y at ground level
                        agentPos.y = agent.GroundY;
                        transform.SetPosition(agentPos);

                        // Get velocity for rotation and animation
                        velocity = crowd->GetAgentVelocity(agent.CrowdAgentId);
                    } else {
                        // No active destination - reset crowd target and sync position
                        crowd->ResetAgentTarget(agent.CrowdAgentId);
                        agent.PathWaypoints.clear();
                        agent.CurrentWaypointIndex = 0;

                        agentPos = currentPos;
                        agentPos.y = agent.GroundY;
                        crowd->SetAgentPosition(agent.CrowdAgentId, agentPos);
                        velocity = glm::vec3(0.0f);
                    }

                    const float speed = glm::length(velocity);
                    agent.CurrentSpeed = speed;
                    agent.CurrentVelocity = velocity;

                    // Rotate towards movement direction (skip on first frame after landing to preserve air rotation)
                    if (!agent.JustLanded) {
                        const float distToDest = glm::length(glm::vec2(agent.Destination.x - agentPos.x, agent.Destination.z - agentPos.z));
                        if (speed > agent.MaxSpeed * 0.5f && distToDest > 0.1f) {
                            const float targetYaw = glm::degrees(std::atan2(velocity.x, velocity.z));
                            auto rotation = transform.GetEulerRotation();

                            float angleDiff = targetYaw - rotation.y;
                            while (angleDiff > 180.0f) angleDiff -= 360.0f;
                            while (angleDiff < -180.0f) angleDiff += 360.0f;

                            rotation.y += angleDiff * std::min(1.0f, agent.RotationSpeed * deltaTime * (speed / agent.MaxSpeed));
                            transform.SetEulerRotation(rotation);
                        }
                    }
                    agent.JustLanded = false;

                    // Update path visualization
                    if (!agent.PathWaypoints.empty()) {
                        while (agent.CurrentWaypointIndex < static_cast<int>(agent.PathWaypoints.size())) {
                            const auto& wp = agent.PathWaypoints[agent.CurrentWaypointIndex];
                            const float dist = glm::length(glm::vec2(wp.x - agentPos.x, wp.z - agentPos.z));
                            if (dist < 0.5f) {
                                agent.CurrentWaypointIndex++;
                            } else {
                                break;
                            }
                        }
                    }
                }
            }
        }

        _navigationManager->Update(deltaTime);
    }
};
