#pragma once
#include "../esc_core.hpp"
#include "random_navigation_component.hpp"
#include "navmesh_agent_component.hpp"
#include "../transform/transform_component.hpp"
#include "../time/time_component.hpp"
#include "../../navigation/navigation_manager.hpp"
#include <random>
#include <memory>

class RandomNavigationSystem final : public ISystemView<RandomNavigationComponent, NavmeshAgentComponent, TransformComponentV2> {
private:
    decltype(std::declval<entt::registry&>().view<DeltaTimeComponent>()) _timeView;
    std::shared_ptr<NavigationManager> _navigationManager;
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _dist;

public:
    RandomNavigationSystem(
        TypeView view,
        decltype(std::declval<entt::registry&>().view<DeltaTimeComponent>()) timeView,
        std::shared_ptr<NavigationManager> navigationManager)
        : ISystemView(view), _timeView(timeView), _navigationManager(std::move(navigationManager)),
          _rng(std::random_device{}()), _dist(0.0f, 1.0f) {}

    void OnTick() override {
        if (!_navigationManager) {
            return;
        }

        auto navmesh = _navigationManager->GetNavmesh();
        if (!navmesh) {
            return;
        }

        float deltaTime = 0.0f;
        for (auto [entity, time] : _timeView.each()) {
            deltaTime = time.Delta;
        }

        // Get navmesh bounds
        const glm::vec3 origin = navmesh->GetOrigin();
        const float cellSize = navmesh->GetCellSize();
        const int width = navmesh->GetWidth();
        const int height = navmesh->GetHeight();

        const float minX = origin.x;
        const float maxX = origin.x + width * cellSize;
        const float minZ = origin.z;
        const float maxZ = origin.z + height * cellSize;

        for (auto [entity, randomNav, agent, transform] : View.each()) {
            if (!agent.Enabled) continue;

            if (!agent.HasDestination) {
                // Generate random position within navmesh bounds
                const float randX = minX + (maxX - minX) * _dist(_rng);
                const float randZ = minZ + (maxZ - minZ) * _dist(_rng);
                const glm::vec3 randomPos(randX, 0.0f, randZ);

                // Find closest walkable point (in case random position is unwalkable)
                const glm::vec3 walkablePos = navmesh->FindClosestWalkablePoint(randomPos);

                agent.Destination = walkablePos;
                agent.HasDestination = true;
                randomNav.CurrentWaitTime = 0.0f;
            } else {
                const glm::vec3 currentPos = transform.GetPosition();
                const float distToDestination = glm::length(agent.Destination - currentPos);

                if (distToDestination < 1.0f) {
                    randomNav.CurrentWaitTime += deltaTime;

                    if (randomNav.CurrentWaitTime >= randomNav.WaitTime) {
                        agent.HasDestination = false;
                    }
                }
            }
        }
    }
};
