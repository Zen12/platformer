#pragma once
#include "../../../entity/component_factory.hpp"
#include "character_input_component.hpp"
#include "character_input_component_serialization.hpp"

class CharacterInputComponentFactory final : public ComponentFactory<CharacterInputComponent, CharacterInputComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<CharacterInputComponent> &component, const CharacterInputComponentSerialization &serialization) override {
        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {
                comp->SetInputSystem(scene->GetInputSystem());
                if (const auto render = scene->GetRenderPipeline().lock()) {
                    comp->SetRenderPipeline(render);
                }
            }
        }
    }
};
