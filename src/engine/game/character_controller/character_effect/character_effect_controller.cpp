#include "character_effect_controller.hpp"

void CharacterEffectController::Update([[maybe_unused]] const float &deltaTime) {
}

void CharacterEffectController::EmitMuzzleFlash(const ShootResult &shootResult) const {
    if (const auto particles = _particles.lock()) {
        constexpr glm::vec3 axis(0.0f, 0.0f, 1.0f);
        constexpr float angle1 = glm::radians(30.0f);
        constexpr float angle2 = glm::radians(-30.0f);

        const glm::vec3 rotated1 = glm::rotate(shootResult.Direction, angle1, axis);
        const glm::vec3 rotated2 = glm::rotate(shootResult.Direction, angle2, axis);

        constexpr float speed = 1.0f;

        particles->OverrideDataVelocity(rotated1 * speed, rotated2 * speed);
        particles->Emit(shootResult.StartPosition);
    }
}

void CharacterEffectController::EmitImpact(const ShootResult &shootResult) const {
    if (const auto particles = _particles.lock()) {
        constexpr glm::vec3 axis(0.0f, 0.0f, 1.0f);
        constexpr float angle1 = glm::radians(30.0f);
        constexpr float angle2 = glm::radians(-30.0f);

        const glm::vec3 rotated1 = glm::rotate(shootResult.Normal, angle1, axis);
        const glm::vec3 rotated2 = glm::rotate(shootResult.Normal, angle2, axis);

        constexpr float speed = 1.0f;

        particles->OverrideDataVelocity(rotated1 * speed, rotated2 * speed);
        particles->Emit(shootResult.Point);
    }
}
