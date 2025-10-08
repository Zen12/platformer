#include "character_input_component.hpp"

void CharacterInputComponent::Update([[maybe_unused]] const float &deltaTime) {
}

glm::vec3 CharacterInputComponent::GetMousePosition() const {
    if (const auto input = _inputSystem.lock()) {
        if (const auto render = _renderPipeline.lock()) {
            const auto mousePosition = input->GetMouseScreenSpace();
            return render->ScreenToWorldPoint(mousePosition);
        }
    }
    return {0.0f, 0.0f, 0.0f};
}

CharacterInputData CharacterInputComponent::GetInputData() const {
    CharacterInputData data;

    if (const auto input = _inputSystem.lock()) {
        glm::vec3 direction = glm::vec3(0);

        if (input->IsKeyPressing(InputKey::A) || input->IsKeyPress(InputKey::A)) {
            direction = glm::vec3(-1, 0, 0);
        } else if (input->IsKeyPressing(InputKey::D) || input->IsKeyPress(InputKey::D)) {
            direction = glm::vec3(1, 0, 0);
        }

        if (input->IsKeyPress(InputKey::Space)) {
            direction.y = 1;
            data.Jump = true;
        }

        data.MovementDirection = direction;
        data.LookAtPosition = GetMousePosition();

        if (input->IsMousePress(MouseButton::Left)) {
            data.Shoot = true;
        }
    }

    return data;
}
