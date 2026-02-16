#pragma once
#include "player_controller_component.hpp"
#include "../transform/transform_component.hpp"
#include "../navmesh_agent/navmesh_agent_component.hpp"
#include "../animation/fsm_animation_component.hpp"
#include "../time/time_component.hpp"
#include "../esc_core.hpp"
#include "scene.hpp"
#include "input_system.hpp"
#include "navigation_manager.hpp"
#include <glm/glm.hpp>
#include <cmath>

class PlayerControllerSystem final : public ISystemView<PlayerControllerComponent, NavmeshAgentComponent, TransformComponentV2> {
private:
    using TypeDeltaTime = decltype(std::declval<entt::registry>().view<DeltaTimeComponent>());

    const TypeDeltaTime _deltaTimeView;
    const std::weak_ptr<Scene> _scene;
    entt::registry& _registry;

public:
    PlayerControllerSystem(
        const TypeView& view,
        const TypeDeltaTime& deltaTimeView,
        const std::weak_ptr<Scene>& scene,
        entt::registry& registry)
        : ISystemView(view), _deltaTimeView(deltaTimeView), _scene(scene), _registry(registry) {}

    void OnTick() override {
        auto scene = _scene.lock();
        if (!scene) return;

        auto inputSystem = scene->GetInputSystem().lock();
        if (!inputSystem) return;

        float deltaTime = 0.0f;
        for (auto [entity, time] : _deltaTimeView.each()) {
            deltaTime = time.Delta;
        }

        for (auto [entity, controller, navAgent, transform] : View.each()) {
            // Check if movement is disabled (e.g., during landing animation)
            bool movementDisabled = false;
            if (const auto* animComp = _registry.try_get<FsmAnimationComponent>(entity)) {
                movementDisabled = animComp->MovementDisabled;
            }

            // Read WASD input (disabled during certain animations)
            glm::vec3 inputDir{0.0f};

            if (!movementDisabled) {
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
            }

            bool hasInput = glm::length(inputDir) > 0.01f;
            bool wasMoving = controller.HasInput();

            // Handle jump input (also disabled during certain animations)
            if (!movementDisabled && inputSystem->IsKeyPress(InputKey::Space) && navAgent.IsGrounded) {
                navAgent.IsJumping = true;
                navAgent.IsGrounded = false;
                navAgent.VerticalVelocity = navAgent.JumpForce;
            }

            // Handle movement based on grounded state
            if (navAgent.IsGrounded) {
                // Grounded: use navmesh navigation
                if (hasInput) {
                    inputDir = glm::normalize(inputDir);
                    controller.SetInputDirection(inputDir);
                    controller.SetHasInput(true);

                    glm::vec3 currentPos = transform.GetPosition();
                    float destDist = controller.GetDestinationDistance();
                    glm::vec3 destination = currentPos + inputDir * destDist;

                    navAgent.Destination = destination;
                    navAgent.HasDestination = true;
                    navAgent.DestinationChanged = true;
                    navAgent.MaxSpeed = controller.GetMoveSpeed();
                } else if (wasMoving) {
                    controller.SetHasInput(false);
                    glm::vec3 currentPos = transform.GetPosition();
                    navAgent.Destination = currentPos;
                    navAgent.HasDestination = false;
                    navAgent.DestinationChanged = true;
                }
            } else {
                // Airborne: direct movement with air control
                if (hasInput) {
                    inputDir = glm::normalize(inputDir);
                    controller.SetInputDirection(inputDir);
                    controller.SetHasInput(true);

                    // Apply air movement directly to position
                    float airSpeed = controller.GetMoveSpeed() * navAgent.AirControlMultiplier;
                    glm::vec3 airVelocity = inputDir * airSpeed;

                    glm::vec3 currentPos = transform.GetPosition();
                    currentPos.x += airVelocity.x * deltaTime;
                    currentPos.z += airVelocity.z * deltaTime;
                    transform.SetPosition(currentPos);

                    // Store velocity for animation
                    navAgent.CurrentVelocity = airVelocity;
                } else {
                    controller.SetHasInput(false);
                    navAgent.CurrentVelocity = glm::vec3(0.0f);
                    // Clear destination so player doesn't move toward old target after landing
                    navAgent.HasDestination = false;
                }
            }

        }
    }
};
