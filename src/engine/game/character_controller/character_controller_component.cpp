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

                        if (const auto effects = _effectController.lock()) {
                            effects->EmitMuzzleFlash(result);
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
                                if (const auto effects = _effectController.lock()) {
                                    effects->EmitImpact(result);
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
