#pragma once
#include "character_animation_component_serialization.hpp"
#include "../../../entity/component_factory.hpp"
#include "../../../scene/scene.hpp"
#include "../movement/character_movement_component.hpp"

class CharacterAnimationComponentFactory final : public ComponentFactory<CharacterAnimationComponent, CharacterAnimationComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<CharacterAnimationComponent> &component, const CharacterAnimationComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {
                if (const auto render = scene->GetRenderPipeline().lock()) {
                    if (const auto entity = _entity.lock()) {
                        comp->SetTransform(entity->GetComponent<Transform>());
                        comp->SetCharacterMovementComponent(entity->GetComponent<CharacterMovementComponent>());
                        comp->SetRenderer(entity->GetComponent<SpineRenderer>());
                    }
                }
            }
        }
    }
};
