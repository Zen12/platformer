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

            // Initialize GroundY and CurrentElevation from navmesh on first tick
            if (agent.GroundY == 0.0f) {
                agent.GroundY = navmesh->GetOrigin().y;
                // Initialize elevation based on starting position
                int startElevation = navmesh->GetElevationAt(currentPos);
                agent.CurrentElevation = (startElevation > 0) ? startElevation : 1;
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

                // Calculate landing Y using visual Y (handles odd/even elevation system)
                const int landingElevation = navmesh->GetElevationAt(currentPos);
                const float landingY = navmesh->GetVisualY(currentPos, agent.GroundY, agent.ElevationHeight);

                // Check for landing on navmesh (use elevation-aware Y)
                if (currentPos.y <= landingY && isOverNavmesh && landingElevation > 0) {
                    // Land on navmesh at correct elevation
                    currentPos.y = landingY;
                    agent.CurrentElevation = landingElevation;
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
                    // Calculate respawn Y using visual Y (handles odd/even elevation system)
                    const float respawnY = navmesh->GetVisualY(respawn, agent.GroundY, agent.ElevationHeight);
                    currentPos.y = respawnY + 3.0f;  // Spawn 3 units above correct elevation
                    agent.IsGrounded = false;  // Will fall down
                    agent.IsJumping = false;
                    agent.VerticalVelocity = 0.0f;
                    crowd->SetAgentPosition(agent.CrowdAgentId, glm::vec3(currentPos.x, respawnY, currentPos.z));
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

                        agent.DestinationChanged = false;

                        // Calculate A* path around obstacles
                        agent.PathWaypoints = navmesh->FindPath(currentPos, clampedDest);
                        agent.CurrentWaypointIndex = 0;

                        // Set crowd target to first waypoint (not final destination)
                        if (!agent.PathWaypoints.empty()) {
                            // Skip waypoint 0 if it's too close (it's usually the start position)
                            int targetWaypoint = agent.PathWaypoints.size() > 1 ? 1 : 0;
                            agent.CurrentWaypointIndex = targetWaypoint;
                            crowd->SetAgentTarget(agent.CrowdAgentId, agent.PathWaypoints[targetWaypoint]);
                        } else {
                            crowd->SetAgentTarget(agent.CrowdAgentId, clampedDest);
                        }
                    }

                    glm::vec3 agentPos;
                    glm::vec3 velocity;

                    // Player-controlled agents: move directly, ignore RVO2 velocity
                    if (agent.IgnoreCrowdVelocity) {
                        if (agent.HasDestination) {
                            // Calculate direction to destination
                            glm::vec3 toTarget = agent.Destination - currentPos;
                            toTarget.y = 0.0f;
                            float distToTarget = glm::length(toTarget);

                            if (distToTarget > 0.1f) {
                                // Move directly toward destination at max speed
                                glm::vec3 moveDir = toTarget / distToTarget;
                                float moveSpeed = agent.MaxSpeed;
                                float moveAmount = std::min(moveSpeed * deltaTime, distToTarget);

                                agentPos = currentPos + moveDir * moveAmount;

                                // Resolve collision with other agents (player can't walk through them)
                                agentPos = crowd->ResolveCollision(agentPos, agent.Radius, agent.CrowdAgentId);

                                // Check walkable AND elevation compatibility
                                if (!navmesh->IsWalkable(agentPos) || !navmesh->CanMoveBetween(currentPos, agentPos)) {
                                    // Stay at current position - can't move there
                                    agentPos = currentPos;
                                } else {
                                    // Update elevation and Y position based on new cell
                                    int newElevation = navmesh->GetElevationAt(agentPos);
                                    if (newElevation > 0) {
                                        agent.CurrentElevation = newElevation;
                                    }
                                }

                                // Set Y based on position (with blending for transitions)
                                agentPos.y = navmesh->GetVisualY(agentPos, agent.GroundY, agent.ElevationHeight);
                                transform.SetPosition(agentPos);

                                // Calculate actual velocity based on movement
                                glm::vec3 actualMove = agentPos - currentPos;
                                actualMove.y = 0.0f;
                                velocity = actualMove / deltaTime;
                            } else {
                                // Arrived at destination
                                agentPos = currentPos;
                                agentPos.y = navmesh->GetVisualY(agentPos, agent.GroundY, agent.ElevationHeight);
                                velocity = glm::vec3(0.0f);
                            }
                        } else {
                            agentPos = currentPos;
                            agentPos.y = navmesh->GetVisualY(agentPos, agent.GroundY, agent.ElevationHeight);
                            velocity = glm::vec3(0.0f);
                        }

                        // Sync position to crowd so AI agents can see and avoid player
                        crowd->SetAgentPosition(agent.CrowdAgentId, agentPos);
                        crowd->ResetAgentTarget(agent.CrowdAgentId);  // Player doesn't use crowd pathfinding
                    }
                    // AI agents: use RVO2 crowd velocity
                    else if (agent.HasDestination) {
                        // Update transform from crowd
                        agentPos = crowd->GetAgentPosition(agent.CrowdAgentId);

                        // Stay on navmesh - if RVO pushed us off or elevation incompatible, stay at current position
                        if (!navmesh->IsWalkable(agentPos) || !navmesh->CanMoveBetween(currentPos, agentPos)) {
                            // Revert to current position - don't teleport
                            agentPos = currentPos;
                            crowd->SetAgentPosition(agent.CrowdAgentId, agentPos);
                        } else {
                            // Update elevation based on new position
                            int newElevation = navmesh->GetElevationAt(agentPos);
                            if (newElevation > 0) {
                                agent.CurrentElevation = newElevation;
                            }
                        }

                        // Set Y based on position (with blending for transitions)
                        agentPos.y = navmesh->GetVisualY(agentPos, agent.GroundY, agent.ElevationHeight);
                        transform.SetPosition(agentPos);

                        // Get velocity for rotation and animation
                        velocity = crowd->GetAgentVelocity(agent.CrowdAgentId);
                    } else {
                        // No active destination - reset crowd target and sync position
                        crowd->ResetAgentTarget(agent.CrowdAgentId);
                        agent.PathWaypoints.clear();
                        agent.CurrentWaypointIndex = 0;

                        agentPos = currentPos;
                        agentPos.y = navmesh->GetVisualY(agentPos, agent.GroundY, agent.ElevationHeight);
                        crowd->SetAgentPosition(agent.CrowdAgentId, agentPos);
                        velocity = glm::vec3(0.0f);
                    }

                    const float speed = glm::length(velocity);
                    agent.CurrentSpeed = speed;
                    agent.CurrentVelocity = velocity;

                    // Rotate towards movement direction (skip on first frame after landing to preserve air rotation)
                    if (!agent.JustLanded) {
                        const float distToDest = glm::length(glm::vec2(agent.Destination.x - agentPos.x, agent.Destination.z - agentPos.z));

                        // For AI agents: rotate towards destination even when slow
                        // For player: rotate towards velocity when moving fast enough
                        if (agent.IgnoreCrowdVelocity) {
                            // Player-controlled: use velocity direction, only when moving
                            if (speed > agent.MaxSpeed * 0.5f && distToDest > 0.1f) {
                                const float targetYaw = glm::degrees(std::atan2(velocity.x, velocity.z));
                                auto rotation = transform.GetEulerRotation();

                                float angleDiff = targetYaw - rotation.y;
                                while (angleDiff > 180.0f) angleDiff -= 360.0f;
                                while (angleDiff < -180.0f) angleDiff += 360.0f;

                                rotation.y += angleDiff * std::min(1.0f, agent.RotationSpeed * deltaTime * (speed / agent.MaxSpeed));
                                transform.SetEulerRotation(rotation);
                            }
                        } else {
                            // AI agents: always rotate towards destination/waypoint
                            if (distToDest > 0.1f && agent.HasDestination) {
                                // Use direction to current waypoint or destination
                                glm::vec3 targetPos = agent.Destination;
                                if (!agent.PathWaypoints.empty() && agent.CurrentWaypointIndex < static_cast<int>(agent.PathWaypoints.size())) {
                                    targetPos = agent.PathWaypoints[agent.CurrentWaypointIndex];
                                }

                                const glm::vec3 toTarget = targetPos - agentPos;
                                const float targetYaw = glm::degrees(std::atan2(toTarget.x, toTarget.z));
                                auto rotation = transform.GetEulerRotation();

                                float angleDiff = targetYaw - rotation.y;
                                while (angleDiff > 180.0f) angleDiff -= 360.0f;
                                while (angleDiff < -180.0f) angleDiff += 360.0f;

                                // Constant rotation speed for AI (not scaled by movement speed)
                                rotation.y += angleDiff * std::min(1.0f, agent.RotationSpeed * deltaTime);
                                transform.SetEulerRotation(rotation);
                            }
                        }
                    }
                    agent.JustLanded = false;

                    // Update waypoint navigation - advance to next waypoint when reached
                    if (!agent.PathWaypoints.empty() && !agent.IgnoreCrowdVelocity) {
                        bool waypointAdvanced = false;
                        while (agent.CurrentWaypointIndex < static_cast<int>(agent.PathWaypoints.size())) {
                            const auto& wp = agent.PathWaypoints[agent.CurrentWaypointIndex];
                            const float dist = glm::length(glm::vec2(wp.x - agentPos.x, wp.z - agentPos.z));
                            if (dist < 0.5f) {
                                agent.CurrentWaypointIndex++;
                                waypointAdvanced = true;
                            } else {
                                break;
                            }
                        }

                        // Update crowd target to next waypoint
                        if (waypointAdvanced && agent.CurrentWaypointIndex < static_cast<int>(agent.PathWaypoints.size())) {
                            crowd->SetAgentTarget(agent.CrowdAgentId, agent.PathWaypoints[agent.CurrentWaypointIndex]);
                        }
                    }
                }
            }
        }

        _navigationManager->Update(deltaTime);
    }
};
