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

            if (agent.CrowdAgentId == -1) {
                agent.CrowdAgentId = crowd->AddAgent(transform.GetPosition(), agent.Radius, agent.MaxSpeed);
            }

            auto crowdAgent = crowd->GetAgent(agent.CrowdAgentId);
            if (!crowdAgent) continue;

            if (agent.HasDestination && crowdAgent->Path.empty()) {
                const auto path = navmesh->FindPath(transform.GetPosition(), agent.Destination);
                if (!path.empty()) {
                    crowd->SetAgentPath(agent.CrowdAgentId, path);
                } else {
                    agent.HasDestination = false;
                }
            }

            transform.SetPosition(crowdAgent->Position);

            // Rotate agent towards movement direction
            const float speed = glm::length(crowdAgent->Velocity);
            if (speed > 0.01f) {
                const float targetYaw = glm::degrees(std::atan2(crowdAgent->Velocity.x, crowdAgent->Velocity.z));
                auto rotation = transform.GetEulerRotation();
                rotation.y = targetYaw;
                transform.SetEulerRotation(rotation);
            }
        }

        _navigationManager->Update(deltaTime);
    }
};
