#include "character_controller_component.hpp"
#include "../health/health_component.hpp"

#define CHARACTER_CONTROLLER_DEBUG 0

CharacterController::CharacterController(const std::weak_ptr<Entity> &entity): Component(entity) {
    _transform = _entity.lock()->GetComponent<Transform>();
}

void CharacterController::Update([[maybe_unused]] const float &deltaTime) {
    if (const auto inputComp = _inputComponent.lock()) {
        if (const auto animation = _animation.lock()) {
            if (const auto movement = _characterMovement.lock()) {
                const auto inputData = inputComp->GetInputData();

                if (inputData.Shoot) {
                    if (const auto shoot = _shootComponent.lock()) {
                        const auto result = shoot->Shoot(inputData.LookAtPosition);
                        if (const auto particles = _particles.lock()) {
                            constexpr glm::vec3 axis(0.0f, 0.0f, 1.0f);
                            constexpr float angle1 = glm::radians(30.0f);
                            constexpr float angle2 = glm::radians(-30.0f);

                            const glm::vec3 rotated1 = glm::rotate(result.Direction, angle1, axis);
                            const glm::vec3 rotated2 = glm::rotate(result.Direction, angle2, axis);

                            constexpr float speed = 1.0f;

                            particles->OverrideDataVelocity(rotated1 * speed, rotated2 * speed);
                            particles->Emit(result.StartPosition);
                        }

                        if (result.IsHit) {
                            // Apply damage to hit entity
                            if (const auto rigid = result.Rigidbody.lock()) {
                                if (const auto entity = rigid->GetEntity().lock()) {
                                    if (const auto health = entity->GetComponent<HealthComponent>().lock()) {
                                        health->DecreaseHealth(_characterSettings.Damage);
                                    }
                                }

                                // Emit impact particles
                                if (const auto particles = _particles.lock()) {
                                    constexpr glm::vec3 axis(0.0f, 0.0f, 1.0f);
                                    constexpr float angle1 = glm::radians(30.0f);
                                    constexpr float angle2 = glm::radians(-30.0f);

                                    const glm::vec3 rotated1 = glm::rotate(result.Normal, angle1, axis);
                                    const glm::vec3 rotated2 = glm::rotate(result.Normal, angle2, axis);

                                    constexpr float speed = 1.0f;

                                    particles->OverrideDataVelocity(rotated1 * speed, rotated2 * speed);
                                    particles->Emit(result.Point);
                                }
                            }
                        }
                    }
                }

                movement->SetDirection(inputData.MovementDirection);
                animation->SetLookAt(inputData.LookAtPosition);
            }
        }
    }
}
