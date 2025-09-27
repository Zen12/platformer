#pragma once
#include "../../asset/factories/component_factory.hpp"
#include "ai_controller_component.hpp"
#include "ai_controller_component_serialization.hpp"

class AiControllerFactory final : public ComponentFactory<AiController, AiControllerComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<AiController> &component, const AiControllerComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {

                const CharacterControllerSettings characterSettings =
                    {
                    serialization.MaxMovementSpeed,
                    serialization.AccelerationSpeed,
                    serialization.DecelerationSpeed,
                    serialization.JumpHeigh,
                    serialization.JumpDuration,
                    serialization.JumpDownMultiplier,
                    serialization.AirControl,
                    serialization.Damage,
                    serialization.AiTargetTransformTag};

                comp->SetCharacterControllerSettings(characterSettings);
                comp->SetInputSystem(scene->GetInputSystem());
                comp->SetPhysicsWorld(scene->GetPhysicsWorld());
                comp->SetSpineRenderer(_entity.lock()->GetComponent<SpineRenderer>());
                comp->SetPathFinder(scene->FindByTag(serialization.PathFinderTag).lock()->GetComponent<AStarFinderComponent>());
                comp->SetTarget(scene->FindByTag(characterSettings.AiTargetTransformTag).lock()->GetComponent<Transform>().lock());
                comp->SetGrid(scene->FindByTag(serialization.GridTag).lock()->GetComponent<GridComponent>());
            }
        }
    }
};