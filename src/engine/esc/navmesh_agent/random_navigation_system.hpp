#pragma once
#include "../esc_core.hpp"
#include "random_navigation_component.hpp"
#include "navmesh_agent_component.hpp"
#include "../transform/transform_component.hpp"
#include "../time/time_component.hpp"
#include <random>

class RandomNavigationSystem final : public ISystemView<RandomNavigationComponent, NavmeshAgentComponent, TransformComponentV2> {
private:
    decltype(std::declval<entt::registry&>().view<DeltaTimeComponent>()) _timeView;
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _dist;

public:
    RandomNavigationSystem(
        TypeView view,
        decltype(std::declval<entt::registry&>().view<DeltaTimeComponent>()) timeView)
        : ISystemView(view), _timeView(timeView), _rng(std::random_device{}()), _dist(-1.0f, 1.0f) {}

    void OnTick() override {
        float deltaTime = 0.0f;
        for (auto [entity, time] : _timeView.each()) {
            deltaTime = time.Delta;
        }

        for (auto [entity, randomNav, agent, transform] : View.each()) {
            if (!agent.Enabled) continue;

            if (!agent.HasDestination) {
                const float x = randomNav.MinX + (randomNav.MaxX - randomNav.MinX) * (_dist(_rng) * 0.5f + 0.5f);
                const float z = randomNav.MinZ + (randomNav.MaxZ - randomNav.MinZ) * (_dist(_rng) * 0.5f + 0.5f);

                agent.Destination = glm::vec3(x, 0.0f, z);
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
