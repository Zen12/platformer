#pragma once
#include "../../entity/component_factory.hpp"
#include "character_controller_component.hpp"
#include "character_controller_component_serialization.hpp"

class CharacterControllerFactory final : public ComponentFactory<CharacterController, CharacterControllerComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<CharacterController> &component, const CharacterControllerComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {
                if (const auto render = scene->GetRenderPipeline().lock()) {
                    if (const auto entity = _entity.lock()) {
                        const CharacterControllerSettings characterSettings =
                        {
                            serialization.Damage,
                            std::string{}
                        };

                        comp->SetCharacterControllerSettings(characterSettings);
                        comp->SetInputSystem(scene->GetInputSystem());
                        comp->SetPhysicsWorld(scene->GetPhysicsWorld());
                        comp->SetParticles(entity->GetComponent<ParticleEmitterComponent>());
                        comp->SetRenderPipeline(render);
                        comp->SetCharacterMovement(entity->GetComponent<CharacterMovementComponent>());
                        comp->SetAnimation(entity->GetComponent<CharacterAnimationComponent>());
                        comp->SetShootComponent(entity->GetComponent<ShootComponent>());
                    }
                }
            }
        }
    }
};