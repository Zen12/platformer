#pragma once
#include "camera_controller_component.hpp"
#include "../transform/transform_component.hpp"
#include "../time/time_component.hpp"
#include "../esc_core.hpp"
#include "../../scene/scene.hpp"
#include "../../system/input_system.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

class CameraControllerSystem final : public ISystemView<CameraControllerComponent, TransformComponentV2> {
private:
    using TypeDeltaTime = decltype(std::declval<entt::registry>().view<DeltaTimeComponent>());
    const TypeDeltaTime _deltaTimeView;
    const std::weak_ptr<Scene> _scene;

public:
    CameraControllerSystem(
        const TypeView &view,
        const TypeDeltaTime &deltaTimeView,
        const std::weak_ptr<Scene> &scene)
        : ISystemView(view), _deltaTimeView(deltaTimeView), _scene(scene) {}

    void OnTick() override {
        if (const auto scene = _scene.lock()) {
            if (const auto inputSystem = scene->GetInputSystem().lock()) {
                // Get delta time
                float deltaTime = 0.02f;
                for (const auto &[_, dt] : _deltaTimeView.each()) {
                    deltaTime = dt.Delta;
                    break;
                }

                for (const auto &[_, controller, transform] : View.each()) {
                    const float moveSpeed = controller.GetMoveSpeed();
                    const float mouseSensitivity = controller.GetMouseSensitivity();

                    // Get current rotation
                    glm::vec3 rotation = transform.GetEulerRotation();

                    // Calculate forward, right, and up vectors based on rotation
                    const glm::quat orientation = glm::quat(glm::vec3(
                        glm::radians(rotation.x),
                        glm::radians(rotation.y),
                        glm::radians(rotation.z)
                    ));

                    const glm::vec3 forward = glm::normalize(orientation * glm::vec3(0.0f, 0.0f, -1.0f));
                    const glm::vec3 right = glm::normalize(orientation * glm::vec3(1.0f, 0.0f, 0.0f));
                    const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

                    // WASD movement
                    glm::vec3 movement(0.0f);

                    if (inputSystem->IsKeyPressing(InputKey::W)) {
                        movement += forward;
                    }
                    if (inputSystem->IsKeyPressing(InputKey::S)) {
                        movement -= forward;
                    }
                    if (inputSystem->IsKeyPressing(InputKey::A)) {
                        movement -= right;
                    }
                    if (inputSystem->IsKeyPressing(InputKey::D)) {
                        movement += right;
                    }

                    // Apply movement
                    if (glm::length(movement) > 0.0f) {
                        movement = glm::normalize(movement) * moveSpeed * deltaTime;
                        transform.AddPosition(movement);
                    }

                    // Mouse panning with right-click
                    const bool isRightMousePressed = inputSystem->IsMousePressing(MouseButton::Right);
                    const glm::vec2 currentMousePos = inputSystem->GetMouseWindowPosition();

                    if (isRightMousePressed) {
                        if (controller.WasRightMousePressed()) {
                            // Calculate mouse delta
                            const glm::vec2 lastMousePos = controller.GetLastMousePosition();
                            const glm::vec2 mouseDelta = currentMousePos - lastMousePos;

                            // Apply rotation (yaw and pitch)
                            rotation.y -= mouseDelta.x * mouseSensitivity;
                            rotation.x -= mouseDelta.y * mouseSensitivity;

                            // Clamp pitch to prevent flipping
                            rotation.x = glm::clamp(rotation.x, -89.0f, 89.0f);

                            transform.SetEulerRotation(rotation);
                        }
                        controller.SetLastMousePosition(currentMousePos);
                    }

                    controller.SetRightMousePressed(isRightMousePressed);
                }
            }
        }
    }
};
