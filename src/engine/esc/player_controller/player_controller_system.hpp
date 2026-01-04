#pragma once
#include "player_controller_component.hpp"
#include "../transform/transform_component.hpp"
#include "../navmesh_agent/navmesh_agent_component.hpp"
#include "../time/time_component.hpp"
#include "../esc_core.hpp"
#include "../../scene/scene.hpp"
#include "../../system/input_system.hpp"
#include <glm/glm.hpp>

class PlayerControllerSystem final : public ISystemView<PlayerControllerComponent, NavmeshAgentComponent, TransformComponentV2> {
private:
    using TypeDeltaTime = decltype(std::declval<entt::registry>().view<DeltaTimeComponent>());

    const TypeDeltaTime _deltaTimeView;
    const std::weak_ptr<Scene> _scene;

public:
    PlayerControllerSystem(
        const TypeView& view,
        const TypeDeltaTime& deltaTimeView,
        const std::weak_ptr<Scene>& scene)
        : ISystemView(view), _deltaTimeView(deltaTimeView), _scene(scene) {}

    void OnTick() override {
        auto scene = _scene.lock();
        if (!scene) return;

        auto inputSystem = scene->GetInputSystem().lock();
        if (!inputSystem) return;

        for (auto [entity, controller, navAgent, transform] : View.each()) {
            // Read WASD input
            glm::vec3 inputDir{0.0f};

            if (inputSystem->IsKeyPressing(InputKey::W)) {
                inputDir.z -= 1.0f;  // Forward (negative Z)
            }
            if (inputSystem->IsKeyPressing(InputKey::S)) {
                inputDir.z += 1.0f;  // Backward (positive Z)
            }
            if (inputSystem->IsKeyPressing(InputKey::A)) {
                inputDir.x -= 1.0f;  // Left
            }
            if (inputSystem->IsKeyPressing(InputKey::D)) {
                inputDir.x += 1.0f;  // Right
            }

            bool hasInput = glm::length(inputDir) > 0.01f;
            bool wasMoving = controller.HasInput();

            if (hasInput) {
                // Normalize input direction
                inputDir = glm::normalize(inputDir);
                controller.SetInputDirection(inputDir);
                controller.SetHasInput(true);

                // Calculate destination ahead of current position
                glm::vec3 currentPos = transform.GetPosition();
                float destDist = controller.GetDestinationDistance();

                glm::vec3 destination = currentPos + inputDir * destDist;

                // Set navmesh agent destination
                navAgent.Destination = destination;
                navAgent.HasDestination = true;
                navAgent.DestinationChanged = true;
                navAgent.MaxSpeed = controller.GetMoveSpeed();
            } else if (wasMoving) {
                // Just released input - stop moving by setting destination to current position
                controller.SetHasInput(false);
                glm::vec3 currentPos = transform.GetPosition();
                navAgent.Destination = currentPos;
                navAgent.HasDestination = false;
                navAgent.DestinationChanged = true;
            }
        }
    }
};
