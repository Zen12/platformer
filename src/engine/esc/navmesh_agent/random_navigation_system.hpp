#pragma once
#include "../esc_core.hpp"
#include "random_navigation_component.hpp"
#include "navmesh_agent_component.hpp"
#include "../transform/transform_component.hpp"
#include "../time/time_component.hpp"
#include "../../navigation/navigation_manager.hpp"
#include <random>
#include <memory>
#include <iostream>

class RandomNavigationSystem final : public ISystemView<RandomNavigationComponent, NavmeshAgentComponent, TransformComponentV2> {
private:
    decltype(std::declval<entt::registry&>().view<DeltaTimeComponent>()) _timeView;
    std::shared_ptr<NavigationManager> _navigationManager;
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _dist;
    std::vector<glm::vec3> _sharedWaypoints;

public:
    RandomNavigationSystem(
        TypeView view,
        decltype(std::declval<entt::registry&>().view<DeltaTimeComponent>()) timeView,
        std::shared_ptr<NavigationManager> navigationManager)
        : ISystemView(view), _timeView(timeView), _navigationManager(std::move(navigationManager)),
          _rng(std::random_device{}()), _dist(0.0f, 1.0f) {
        _sharedWaypoints.reserve(10);
    }

    void OnTick() override {
        if (!_navigationManager) {
            return;
        }

        auto navmesh = _navigationManager->GetNavmesh();
        if (!navmesh) {
            return;
        }

        // Generate 10 shared waypoints once
        if (_sharedWaypoints.empty()) {
            const int width = navmesh->GetWidth();
            const int height = navmesh->GetHeight();
            const float cellSize = navmesh->GetCellSize();
            const glm::vec3 origin = navmesh->GetOrigin();

            while (_sharedWaypoints.size() < 10) {
                for (int attempts = 0; attempts < 20; ++attempts) {
                    // Generate random position within navmesh bounds
                    const float randX = origin.x + _dist(_rng) * width * cellSize;
                    const float randZ = origin.z + _dist(_rng) * height * cellSize;
                    const glm::vec3 randomPos(randX, origin.y, randZ);

                    // Find nearest walkable point
                    const glm::vec3 nearestPoint = navmesh->FindNearestPoint(randomPos);

                    // Check if it's actually walkable
                    if (navmesh->IsWalkable(nearestPoint)) {
                        _sharedWaypoints.push_back(nearestPoint);
                        break;
                    }
                }
            }
        }

        float deltaTime = 0.0f;
        for (auto [entity, time] : _timeView.each()) {
            deltaTime = time.Delta;
        }

        auto crowd = _navigationManager->GetCrowd();

        for (auto [entity, randomNav, agent, transform] : View.each()) {
            if (!agent.Enabled) continue;

            // Initialize with random waypoint index on first tick
            if (randomNav.CurrentWaypointIndex == -1) {
                randomNav.CurrentWaypointIndex = static_cast<int>(_dist(_rng) * 10.0f) % 10;
            }

            if (!agent.HasDestination) {
                // Assign next waypoint in the sequence
                agent.Destination = _sharedWaypoints[randomNav.CurrentWaypointIndex];
                agent.HasDestination = true;
                agent.DestinationChanged = true;  // Mark that destination needs to be sent to crowd
                randomNav.CurrentWaitTime = 0.0f;
                randomNav.ReachedDestination = false;
            } else {
                const glm::vec3 currentPos = transform.GetPosition();
                const float distToDestination = glm::length(agent.Destination - currentPos);

                if (distToDestination < 1.0f) {
                    // Stop the agent to prevent oscillation
                    if (!randomNav.ReachedDestination && crowd && agent.CrowdAgentId != -1) {
                        crowd->ResetAgentTarget(agent.CrowdAgentId);
                        randomNav.ReachedDestination = true;
                    }

                    randomNav.CurrentWaitTime += deltaTime;

                    if (randomNav.CurrentWaitTime >= randomNav.WaitTime) {
                        // Advance to next waypoint in sequence (loop back to 0)
                        randomNav.CurrentWaypointIndex = (randomNav.CurrentWaypointIndex + 1) % 10;
                        agent.HasDestination = false;
                    }
                }
            }
        }
    }
};
