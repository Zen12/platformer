#pragma once

#pragma once
#include "character_movement_component.hpp"
#include "character_movement_component_serialization.hpp"
#include "../../../entity/component_factory.hpp"
#include "character_movement_settings.hpp"
#include "../../../scene/scene.hpp"

class CharacterMovementFactory final : public ComponentFactory<CharacterMovementComponent, CharacterMovementComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<CharacterMovementComponent> &component, const CharacterMovementComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {
                if (const auto render = scene->GetRenderPipeline().lock()) {
                    if (const auto entity = _entity.lock()) {
                        const CharacterMovementSettings characterSettings =
                        {
                            serialization.MaxMovementSpeed,
                            serialization.AccelerationSpeed,
                            serialization.DecelerationSpeed,
                            serialization.JumpHeight,
                            serialization.JumpDuration,
                            serialization.JumpDownMultiplier,
                            serialization.AirControl
                        };

                        comp->SetSettings(characterSettings);
                        comp->SetWorld(scene->GetPhysicsWorld());
                        comp->SetTransform(entity->GetComponent<Transform>());
                    }
                }
            }
        }
    }
};
