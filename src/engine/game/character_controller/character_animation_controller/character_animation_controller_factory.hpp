#pragma once
#include "../../../entity/component_factory.hpp"
#include "character_animation_controller.hpp"
#include "character_animation_controller_serialization.hpp"

class CharacterAnimationControllerFactory final : public ComponentFactory<CharacterAnimationController, CharacterAnimationControllerSerialization> {
protected:
    void FillComponent(const std::weak_ptr<CharacterAnimationController> &component, const CharacterAnimationControllerSerialization &serialization) override {
        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {
                if (const auto entity = _entity.lock()) {
                    comp->SetAnimationComponent(entity->GetComponent<CharacterAnimationComponent>());
                    comp->SetMovementComponent(entity->GetComponent<CharacterMovementComponent>());
                    comp->SetTransform(entity->GetComponent<Transform>());
                }
            }
        }
    }
};
