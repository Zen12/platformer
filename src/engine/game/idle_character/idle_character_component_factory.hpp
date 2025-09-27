#pragma once

#include "idle_character_component.hpp"
#include "idle_character_component_serialization.hpp"
#include "../../asset/factories/component_factory.hpp"

class IdleCharacterComponentFactor final : public ComponentFactory<IdleCharacter, IdleCharacterSerialization> {
protected:
    void FillComponent(const std::weak_ptr<IdleCharacter> &component,
        [[maybe_unused]] const IdleCharacterSerialization &serialization) override {

        if (const auto entity = _entity.lock()) {
            if (const auto comp = component.lock()) {
                comp->SetSpineRenderer(entity->GetComponent<SpineRenderer>());
                comp->SetAnimation(serialization.IdleAnimation);
            }
        }
    }
};