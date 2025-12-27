#pragma once
#include "../esc_core.hpp"
#include "mouse_ik_controller_component.hpp"
#include "ozz_ik_component.hpp"
#include "../../system/input_system.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <iostream>

class MouseIKControllerSystem final : public ISystemView<MouseIKControllerComponent, OzzIKComponent> {
private:
    std::shared_ptr<InputSystem> _inputSystem;

public:
    explicit MouseIKControllerSystem(const TypeView& view,
                                    std::shared_ptr<InputSystem> inputSystem)
        : ISystemView(view), _inputSystem(std::move(inputSystem)) {}

    void OnTick() override {
        if (!_inputSystem) return;

        // Get mouse screen space position (normalized -1 to 1)
        glm::vec2 mouseScreen = _inputSystem->GetMouseScreenSpace();

        // Debug log mouse position
        static int logCount = 0;
        if (logCount++ % 60 == 0) {
            std::cout << "[MOUSE_IK] Mouse screen space: (" << mouseScreen.x << ", " << mouseScreen.y << ")" << std::endl;
        }

        // Update all entities with mouse IK controller
        for (const auto& [entity, mouseIK, ik] : View.each()) {
            if (!mouseIK.Enabled || !ik.Enabled) continue;

            // Create a target point in a small area around a point in front
            // Use mouse to offset within a 2x2 area
            glm::vec3 target;
            target.x = mouseScreen.x * 0.5f;  // -0.5 to 0.5 range
            target.y = -5.0f + mouseScreen.y * 0.5f;  // Around character Y with mouse offset
            target.z = -3.0f;  // In front of character (character is at Z=-5)

            // Apply offset
            target += mouseIK.TargetOffset;

            // Debug log
            static int debugCount = 0;
            if (debugCount++ % 60 == 0) {
                std::cout << "[MOUSE_IK] Mouse: (" << mouseScreen.x << ", " << mouseScreen.y << ")"
                          << " Target: (" << target.x << ", " << target.y << ", " << target.z << ")" << std::endl;
            }

            // Update mouse controller world position
            mouseIK.WorldPosition = target;

            // Update IK target
            ik.Target = target;
        }
    }
};
