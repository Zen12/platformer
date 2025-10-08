#include "character_controller_component.hpp"
#include "../health/health_component.hpp"

#define CHARACTER_CONTROLLER_DEBUG 0

CharacterController::CharacterController(const std::weak_ptr<Entity> &entity): Component(entity) {
    _transform = _entity.lock()->GetComponent<Transform>();
}

void CharacterController::SetShootComponent(const std::weak_ptr<ShootComponent> &shootComponent) noexcept {
    _shootComponent = shootComponent;
}


glm::vec3 CharacterController::GetMousePosition() const {

    if (const auto input = _inputSystem.lock()) {
        if (const auto render = _renderPipeline.lock()) {
            const auto mousePosition = input->GetMouseScreenSpace();
            return render->ScreenToWorldPoint(mousePosition);
        }
    }
    return {0.0f, 0.0f, 0.0f};
}



void CharacterController::Update([[maybe_unused]] const float &deltaTime) {
    if (const auto input = _inputSystem.lock()) {
        if (const auto animation = _animation.lock()) {
            if (const auto movement = _characterMovement.lock()) {
                glm::vec3 direction = glm::vec3(0);

                if (input->IsKeyPressing(InputKey::A) || input->IsKeyPress(InputKey::A)) {
                    direction = glm::vec3(-1, 0, 0);
                } else if (input->IsKeyPressing(InputKey::D) || input->IsKeyPress(InputKey::D)) {
                    direction = glm::vec3(1, 0, 0);
                }

                if (input->IsKeyPress(InputKey::Space)) {
                    direction.y = 1;
                }

                const auto mouseWorldPosition = GetMousePosition();

                if (input->IsMousePress(MouseButton::Left)) {
                    if (const auto shoot = _shootComponent.lock()) {
                        const auto result = shoot->Shoot(mouseWorldPosition);
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

                movement->SetDirection(direction);
                animation->SetLookAt(mouseWorldPosition);
            }
        }
    }
}
