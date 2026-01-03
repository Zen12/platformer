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

            // Check if agent is on unwalkable terrain and teleport to closest walkable point
            const glm::vec3 currentPos = transform.GetPosition();
            if (!navmesh->IsWalkable(currentPos)) {
                const glm::vec3 closestWalkable = navmesh->FindNearestPoint(currentPos);
                transform.SetPosition(closestWalkable);
            }

            // Create agent if needed
            if (agent.CrowdAgentId == -1) {
                agent.CrowdAgentId = crowd->AddAgent(transform.GetPosition(), agent.Radius, 2.0f, agent.MaxSpeed);
            }

            // Check if agent is still active
            if (!crowd->IsAgentActive(agent.CrowdAgentId)) {
                continue;
            }

            // Set destination when it changes
            if (agent.HasDestination && agent.DestinationChanged) {
                crowd->SetAgentTarget(agent.CrowdAgentId, agent.Destination);
                agent.DestinationChanged = false;

                // Also cache path for visualization
                agent.PathWaypoints = navmesh->FindPath(currentPos, agent.Destination);
                agent.CurrentWaypointIndex = 0;
            }

            // Update transform from crowd (Detour handles RVO)
            const glm::vec3 agentPos = crowd->GetAgentPosition(agent.CrowdAgentId);
            transform.SetPosition(agentPos);

            // Rotate agent towards movement direction with simple lerp
            const glm::vec3 velocity = crowd->GetAgentVelocity(agent.CrowdAgentId);
            const float speed = glm::length(velocity);
            if (speed > 0.1f) {
                const float targetYaw = glm::degrees(std::atan2(velocity.x, velocity.z));
                auto rotation = transform.GetEulerRotation();

                // Calculate shortest angle difference
                float angleDiff = targetYaw - rotation.y;
                while (angleDiff > 180.0f) angleDiff -= 360.0f;
                while (angleDiff < -180.0f) angleDiff += 360.0f;

                // Simple lerp rotation
                rotation.y += angleDiff * std::min(1.0f, agent.RotationSpeed * deltaTime);

                transform.SetEulerRotation(rotation);
            }

            // Update path visualization waypoint index
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

        _navigationManager->Update(deltaTime);
    }
};
