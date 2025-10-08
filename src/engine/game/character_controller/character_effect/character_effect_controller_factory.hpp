#pragma once
#include "../../../entity/component_factory.hpp"
#include "character_effect_controller.hpp"
#include "character_effect_controller_serialization.hpp"

class CharacterEffectControllerFactory final : public ComponentFactory<CharacterEffectController, CharacterEffectControllerSerialization> {
protected:
    void FillComponent(const std::weak_ptr<CharacterEffectController> &component, const CharacterEffectControllerSerialization &serialization) override {
        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {
                if (const auto entity = _entity.lock()) {
                    comp->SetParticles(entity->GetComponent<ParticleEmitterComponent>());
                }
            }
        }
    }
};
